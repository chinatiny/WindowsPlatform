#include <windows.h>
#include <tchar.h>
#include <stdio.h>


LONG CALLBACK UnhandleFilter(EXCEPTION_POINTERS* pException)
{

	DWORD dwRet = (DWORD)&pException;
	dwRet -= 4;
	dwRet = *(DWORD*)dwRet;
	printf("addr:0x%X\n", dwRet);
	printf("默认异常过滤函数被调用\n");
	system("pause");
	ExitProcess(0);
	return 0;
}

typedef PVOID(__stdcall *fpTypeRtlDecodePointer)(PVOID Pointer);

int main()
{	
	HMODULE hK32 = GetModuleHandle(_T("kernel32.dll"));
	HMODULE hNtdll = GetModuleHandle(_T("ntdll.dll"));
	DWORD dwValue = 0;
	fpTypeRtlDecodePointer  fpRtlDecodePointer = NULL;
	LPVOID lpResultAddr = NULL;


	dwValue = *((DWORD*)((DWORD)hK32 + 0xE0074));
	fpRtlDecodePointer = (fpTypeRtlDecodePointer)GetProcAddress(hNtdll, "RtlDecodePointer");
	lpResultAddr = fpRtlDecodePointer((LPVOID)dwValue);
	printf("默认异常处理函数地址:0x%0X\n", lpResultAddr);
	// 设置顶层过滤处理表达式
	SetUnhandledExceptionFilter(&UnhandleFilter);

	dwValue = *((DWORD*)((DWORD)hK32 + 0xE0074));
	fpRtlDecodePointer = (fpTypeRtlDecodePointer)GetProcAddress(hNtdll, "RtlDecodePointer");
	lpResultAddr = fpRtlDecodePointer((LPVOID)dwValue);
	printf("现在异常函数:0x%0X\n", lpResultAddr);
	if (lpResultAddr == (LPVOID)UnhandleFilter)
	{
		printf("查找到UnhandleFilter\n");
	}



	//设置为非调试状态
// 	__asm
// 	{
// 			mov  eax, dword ptr fs : [0x00000018]
// 			mov         eax, dword ptr[eax + 0x30]
// 			mov  byte ptr[eax + 0x2], 0
// 	}
// 
 	*(int*)0 = 0;
	system("pause");
	return 0;
}
