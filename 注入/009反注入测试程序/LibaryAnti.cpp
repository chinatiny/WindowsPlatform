#include "LibaryAnti.h"
#include <windows.h>
#include <tchar.h>
#include <wchar.h>
#include "Dbg.h"
#include "AntiCommon.h"
#include "ProcessOperation.h"
#include "PEOperation.h"
#include "PEAnti.h"
#include "UnDocoumentApi.h"

#include "detours/detours.h"
#pragma comment(lib,"detours/lib.X86/detours.lib")

// BegEngine在使用的时候需要定义下
#define BEA_ENGINE_STATIC
#define BEA_USE_STDCALL
#include "BeaEngine_4.1/Win32/headers/BeaEngine.h"
#pragma comment (lib , "BeaEngine_4.1/Win32/Win32/Lib/BeaEngine.lib")
// 防止编译错误。
#pragma comment(linker, "/NODEFAULTLIB:\"crt.lib\"")



//////////////////////////////类型定义///////////////////////////////////////

typedef NTSTATUS(WINAPI *fpLdrLoadDll)(
	IN PWCHAR PathToFile OPTIONAL,
	IN ULONG Flags OPTIONAL,
	IN PUNICODE_STRING ModuleFileName,
	OUT PHANDLE ModuleHandle);


typedef HMODULE (WINAPI *fpLoadLibraryA)(
	_In_ LPCSTR lpLibFileName
);

typedef HMODULE (WINAPI *fpLoadLibraryW)(
_In_ LPCWSTR lpLibFileName
);

//////////////////////////////内部函数///////////////////////////////////////
static fpLdrLoadDll s_fpSrcLdrDll = NULL;
static fpLoadLibraryA s_fpLoadLibrayA = NULL;
static fpLoadLibraryW s_fpLoadLibrayW = NULL;
static bool s_bShellCode = false;  //是否：shell code、消息钩子加载动态库、没有使用标准接口加载dll
static bool s_bHidMode = false;   //是否：加载的动态库是否有隐藏模块的动作

//检测函数需要临时保存的
static WCHAR s_szLoadlibrayDllName[MAX_PATH] = { 0 };
static LPVOID s_lpCheckStack = NULL;  //调用LoadlibraryA或者LoadlibraryW的堆栈地址 + 4就是返回地址
static LPVOID s_lpDllNameAddr = NULL; //dll名字的地址
static bool s_bCheckReAddrEnumFuncOk = false; //遍历模块检测调用loadlibary的返回值的辅助变量



//检测loadlibary的调用堆栈，如果调用地址不属于任何模块，肯定是shellcode注入
static bool  CheckReAddrEnumFunc(IN  void *callBackParameter, IN MODULEENTRY32* pModuleEntry)
{
	DWORD dwCmpAddr = (DWORD)callBackParameter;
	IMAGE_NT_HEADERS*pNtHeader = GetNtHeader(pModuleEntry->modBaseAddr);
	IMAGE_SECTION_HEADER* pSectionHeader = NULL;
	pSectionHeader = IMAGE_FIRST_SECTION(pNtHeader);
	for (ULONG i = 0; i < pNtHeader->FileHeader.NumberOfSections; ++i)
	{
		//如果区段是可执行的
		if (IMAGE_SCN_MEM_EXECUTE & pSectionHeader->Characteristics)
		{
			DWORD dwBegin = (DWORD)pModuleEntry->modBaseAddr + pSectionHeader->VirtualAddress;
			DWORD dwSectionCnt = pSectionHeader->SizeOfRawData / pNtHeader->OptionalHeader.SectionAlignment +
				(pSectionHeader->SizeOfRawData % pNtHeader->OptionalHeader.SectionAlignment ? 1 : 0);
			DWORD dwSectionSize = dwSectionCnt *  pNtHeader->OptionalHeader.SectionAlignment;
			DWORD dwEnd = dwBegin + dwSectionSize;
			if (dwCmpAddr >= dwBegin && dwCmpAddr < dwEnd)
			{
				//防范在已知模块中利用模块的空隙加载shellcode
				HANDLE hFile = NULL;
				hFile = CreateFile(pModuleEntry->szExePath,
					GENERIC_READ,
					FILE_SHARE_READ,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL
					);
				if (INVALID_HANDLE_VALUE == hFile)
				{
					return false;
				}
				DWORD	dwLowSize = GetFileSize(hFile, NULL);
				PBYTE pFileBuff = new BYTE[dwLowSize];
				ZeroMemory(pFileBuff, dwLowSize);
				DWORD dwRealRead = 0;
				ReadFile(hFile, pFileBuff, dwLowSize, &dwRealRead, NULL);
				DWORD dwRva = dwCmpAddr - (ULONG)(pModuleEntry->modBaseAddr);
				DWORD dwFoa = 0;
				RVA2Foa(pFileBuff, dwRva, &dwFoa);
				BYTE btOrginValue = *(BYTE*)(pFileBuff + dwFoa);
				BYTE btNowValue = *(BYTE*)dwCmpAddr;
				delete[] pFileBuff;
				CloseHandle(hFile);

				//如果原始数据和现在的数据相等代表正常的loadlibary加载
				if (btOrginValue == btNowValue) s_bCheckReAddrEnumFuncOk = true;
				return true;
			}

		}
		pSectionHeader += 1;
	}
	return false;
}


//检查动态库
static void CheckDll(PWSTR pszDllName)
{
	HMODULE hMod = GetModuleHandleW(pszDllName);
	if (NULL != hMod) return;

	//代表没有使用library加载动态库
	if (wcscmp(pszDllName, s_szLoadlibrayDllName) != 0)
	{
		s_bShellCode = true;
		return;
	}
	//获取返回地址
	DWORD dwRetAddr = *(DWORD*)((DWORD)s_lpCheckStack - 4);

	//遍历当前进程模块，如果返回值不属于任何模块，外挂无疑
	EnumModule(GetCurrentProcessId(), CheckReAddrEnumFunc, (void*)dwRetAddr);
	bool bCheckRetResult = s_bCheckReAddrEnumFuncOk;
	s_bCheckReAddrEnumFuncOk = false;
	if (!bCheckRetResult)  //模块检测是否正常
	{
		__asm int 3
		s_bShellCode = true;
		return;
	}
}


//山寨版本的ldrloaddll
static NTSTATUS WINAPI MyLdrLoadDll(
	IN PWCHAR PathToFile OPTIONAL,
	IN ULONG Flags OPTIONAL,
	IN PUNICODE_STRING ModuleFileName,
	OUT PHANDLE ModuleHandle)
{
	NTSTATUS ntStatus;
	WCHAR szDllName[MAX_PATH];
	ZeroMemory(szDllName, sizeof(szDllName));
	memcpy(szDllName, ModuleFileName->Buffer, ModuleFileName->Length);
	CheckDll(szDllName);
	//在加载之前判断下该模块是否被加载过
	HMODULE hPreMod = GetModuleHandleW(szDllName);
	ntStatus = s_fpSrcLdrDll(PathToFile, Flags, ModuleFileName, ModuleHandle);
	DWORD dwLastError = GetLastError();
	//如果没有被加载过，在执行完s_fpSrcLdrDll成功后检测是否做了隐藏自己的操作
	if (STATUS_SUCCESS == ntStatus &&NULL == hPreMod)
	{
		//GetModuleHandleW其实是读取peb的消息,没必要再遍历一遍了
		HMODULE hNowMod = GetModuleHandleW(szDllName);
		if (NULL == hNowMod) s_bHidMode = true;
		//因为新的dll被加载通知重新计算md5
		CheckModPEInfo();
	}
	//恢复错误码
	SetLastError(dwLastError);
	return ntStatus;
}

//山寨版本的LoadLibaryA
static HMODULE WINAPI  MyLoadLibraryA(
	_In_ LPCSTR lpLibFileName
	)
{
	if (NULL == lpLibFileName) return NULL;

	WCHAR szTmpBuff[MAX_PATH];
	CHAR_TO_WCHAR(lpLibFileName, szTmpBuff);
	wcscpy_s(s_szLoadlibrayDllName, _countof(s_szLoadlibrayDllName), szTmpBuff);
	s_lpDllNameAddr = (LPVOID)lpLibFileName;
	s_lpCheckStack = (LPVOID)&lpLibFileName;
	return s_fpLoadLibrayA(lpLibFileName);
}

//山寨版本的LoadLibraryW
static HMODULE WINAPI MyLoadLibraryW(
	_In_ LPCWSTR lpLibFileName
	)
{
	if (NULL == lpLibFileName) return NULL;
	wcscpy_s(s_szLoadlibrayDllName, _countof(s_szLoadlibrayDllName), lpLibFileName);
	s_lpDllNameAddr = (LPVOID)lpLibFileName;
	s_lpCheckStack = (LPVOID)&lpLibFileName;
	return s_fpLoadLibrayW(lpLibFileName);



}



//////////////////////////////对外接口///////////////////////////////////////
void MonitorLoadDll()
{
	HMODULE hNtDll = LoadLibrary(_T("ntdll.dll"));
	HMODULE hK32 = LoadLibrary(_T("Kernel32.dll"));
	s_fpSrcLdrDll = (fpLdrLoadDll)GetProcAddress(hNtDll, "LdrLoadDll");
	s_fpLoadLibrayA = (fpLoadLibraryA)GetProcAddress(hK32, "LoadLibraryA");
	s_fpLoadLibrayW = (fpLoadLibraryW)GetProcAddress(hK32, "LoadLibraryW");
	//
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((PVOID*)&s_fpSrcLdrDll, MyLdrLoadDll);
	DetourAttach((PVOID*)&s_fpLoadLibrayA, MyLoadLibraryA);
	DetourAttach((PVOID*)&s_fpLoadLibrayW, MyLoadLibraryW);
	DetourTransactionCommit();
}

//判断是否是shellcode注入
bool IsFondShellCode()
{
	return s_bShellCode;
}

bool IsFondModHidSelf()
{
	return s_bHidMode;
}
