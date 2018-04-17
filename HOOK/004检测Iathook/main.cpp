#include <windows.h>
#include <tchar.h>
#include <psapi.h>
#include <stdio.h>

DWORD RvaToOffect(PBYTE pPeFile, DWORD Rva)
{
	//1 得到区段表的起始位置
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pPeFile;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(
		pPeFile + pDos->e_lfanew);

	PIMAGE_SECTION_HEADER pSection =
		IMAGE_FIRST_SECTION(pNt);
	//2 看一下这个Rva落在了哪一个区段里面
	for (int i = 0; i < pNt->FileHeader.NumberOfSections; i++)
	{
		//2.1 假如在头部
		if (Rva < pSection->VirtualAddress){
			return Rva;
		}
		//2.2 假如不在头部 
		//2.2.1算一下当前区段的范围
		DWORD dwAlignment =
			pNt->OptionalHeader.SectionAlignment;
		DWORD dwCount = pSection->Misc.VirtualSize / dwAlignment;
		dwCount += (pSection->Misc.VirtualSize%dwAlignment == 0) ? 0 : 1;
		//2.2.2 假如落在了这个区段中
		if (Rva >= pSection->VirtualAddress&&
			Rva < pSection->VirtualAddress + dwCount*dwAlignment)
		{
			return Rva - pSection->VirtualAddress +
				pSection->PointerToRawData;
		}
		pSection++;
	}
	return 0;
}

//查看某一个模块是否被IATHOOK了
void CheckIAT(HANDLE hProcess, HMODULE hModule)
{
	PBYTE pModuleBuf = NULL;
	DWORD dwReadModuleSize = 0;
	//1 把模块的副本拷贝出来
	//1.1读取出DOS头大小，找到PE头
	pModuleBuf = new BYTE[sizeof(IMAGE_DOS_HEADER)];
	ReadProcessMemory(hProcess, hModule, pModuleBuf, sizeof(IMAGE_DOS_HEADER), &dwReadModuleSize);
	//1.2得到PE头后，得到镜像大小
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pModuleBuf;
	DWORD dwNewFile = pDos->e_lfanew;
	delete[]pModuleBuf;
	DWORD dwHeaderSize = dwNewFile + sizeof(IMAGE_NT_HEADERS);
	pModuleBuf = new BYTE[dwHeaderSize];
	ReadProcessMemory(hProcess, hModule, pModuleBuf, dwHeaderSize, &dwReadModuleSize);
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pModuleBuf + dwNewFile);
	//2 找到导入表
	PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = &pNt->OptionalHeader;
	PIMAGE_DATA_DIRECTORY pImportDir = pOptionalHeader->DataDirectory + 1;
	DWORD dwImportVA = (DWORD)((DWORD)hModule + pImportDir->VirtualAddress);
	DWORD dwSize = pImportDir->Size;
	//3 将导入表读入内存
	PBYTE pImportBuf = new BYTE[dwSize];
	ReadProcessMemory(hProcess, (LPVOID)dwImportVA, pImportBuf, dwSize, &dwReadModuleSize);
	PIMAGE_IMPORT_DESCRIPTOR pImport = PIMAGE_IMPORT_DESCRIPTOR(pImportBuf);
	//3 查找导入表中的INT中的函数名称，去此dll文件的导出表中找到函数地址，
	//根据函数地址RVA+ImageBase看看是否和IAT中的数据相等
	while (pImport->Name != 0)
	{
		PINT pFunction = nullptr;
		//5.1 待检测的IAT与INT
		DWORD pIatRVA = (DWORD)(pImport->FirstThunk + (DWORD)hModule);//IAT
		DWORD pIntRVA = (DWORD)(pImport->OriginalFirstThunk + (DWORD)hModule);//INT
		PIMAGE_THUNK_DATA pIat = (PIMAGE_THUNK_DATA)new BYTE[0x1000];
		PIMAGE_THUNK_DATA pInt = (PIMAGE_THUNK_DATA)new BYTE[0x1000];
		ReadProcessMemory(hProcess, (LPVOID)pIatRVA, pIat, 0x1000, &dwReadModuleSize);
		ReadProcessMemory(hProcess, (LPVOID)pIntRVA, pInt, 0x1000, &dwReadModuleSize);
		//5.2 获得此dll的名字
		char* pDllnameVA = (char*)((DWORD)hModule + pImport->Name);
		char*  pDllname = new char[MAX_PATH];
		ReadProcessMemory(hProcess, (LPVOID)pDllnameVA, pDllname, MAX_PATH, &dwReadModuleSize);
		//5.3 将此dll的文件读入内存
		HANDLE hDllFile = CreateFileA(
			pDllname,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		DWORD dwDllSize = GetFileSize(hDllFile, NULL);
		PBYTE pDllbuf = new BYTE[dwDllSize];
		DWORD dwReadSize = 0;
		ReadFile(hDllFile, pDllbuf, dwDllSize, &dwReadSize, NULL);
		//5.4获得此Dll的导出地址表
		PIMAGE_DOS_HEADER pDestanceDos = (PIMAGE_DOS_HEADER)pDllbuf;
		PIMAGE_NT_HEADERS pDestanceNt = (PIMAGE_NT_HEADERS)(pDllbuf + pDestanceDos->e_lfanew);
		PIMAGE_DATA_DIRECTORY pExportDir = &(pDestanceNt->OptionalHeader.DataDirectory[0]);
		PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)(RvaToOffect(pDllbuf, pExportDir->VirtualAddress) + pDllbuf);
		pFunction = (PINT)(RvaToOffect(pDllbuf, pExport->AddressOfFunctions) + pDllbuf);
		//5.5 使用INT得到的此函数的序号，进而得到导出表中函数地址RVA
		while (pIat->u1.AddressOfData != 0)
		{
			//5.5.1得到序号
			WORD Order = 0;
			if (IMAGE_SNAP_BY_ORDINAL32(pInt->u1.AddressOfData) != 1)
			{
				char* pFunnameVA = (char*)(pInt->u1.ForwarderString + (DWORD)hModule);
				char*  pFunname = new char[MAX_PATH];
				ReadProcessMemory(hProcess, (LPVOID)pFunnameVA, pFunname, MAX_PATH, &dwReadModuleSize);
				Order = PIMAGE_IMPORT_BY_NAME(pFunname)->Hint;
				delete[]pFunname;
			}
			else
			{
				Order = (WORD)pInt->u1.Ordinal;
			}
			//5.5.2得到计算出的地址
			int ExportFunRva = pFunction[Order];
			int ImageBase = (int)hModule;
			int FunVA = ExportFunRva + ImageBase;
			if (FunVA == pInt->u1.Function)
			{
				if (IMAGE_SNAP_BY_ORDINAL32(pInt->u1.AddressOfData) != 1)
				{
					char* pFunnameVA = (char*)(pInt->u1.ForwarderString + (DWORD)hModule);
					char*  pFunname = new char[MAX_PATH];
					ReadProcessMemory(hProcess, (LPVOID)pFunnameVA, pFunname, MAX_PATH, &dwReadModuleSize);
					printf("%s未被Hook", PIMAGE_IMPORT_BY_NAME(pFunname)->Name);
					delete[]pFunname;
				}
			}
			else
			{
				if (IMAGE_SNAP_BY_ORDINAL32(pInt->u1.AddressOfData) != 1)
				{
					char* pFunnameVA = (char*)(pInt->u1.ForwarderString + (DWORD)hModule);
					char*  pFunname = new char[MAX_PATH];
					ReadProcessMemory(hProcess, (LPVOID)pFunnameVA, pFunname, MAX_PATH, &dwReadModuleSize);
					printf("%s被Hook", PIMAGE_IMPORT_BY_NAME(pFunname)->Name);
					delete[]pFunname;
				}
			}
			pInt++;
			pIat++;
		}
		pImport++;
	}

}




int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, 30496);
	DWORD dwNeed1 = 0;
	DWORD dwNeed2 = 0;
	EnumProcessModulesEx(hProcess, NULL, 0, &dwNeed1, LIST_MODULES_ALL);
	HMODULE* pModule = new HMODULE[dwNeed1];
	EnumProcessModulesEx(hProcess, pModule, dwNeed1, &dwNeed2, LIST_MODULES_ALL);
	DWORD dwSize = 0;
	CHAR buf[MAX_PATH] = {};
	PBYTE pModuleBuf = NULL;
	DWORD dwReadModuleSize = 0;
	DWORD i = 0;
	for (; i < dwNeed2; i++)
	{

		GetModuleFileNameExA(hProcess, pModule[i], buf, MAX_PATH);
		char* pLow;
		_strlwr_s(pLow = _strdup(buf), strlen(buf) + 1);
		if (strstr(pLow, "user32.dll") != NULL)
		{
			break;
		}
	}
	CheckIAT(hProcess, pModule[i]);
	CloseHandle(hProcess);
	return 0;
}