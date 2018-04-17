#include "PEOperation.h"
#include <tchar.h>
#include <stdio.h>
#include <time.h>
#include <locale.h>

#define OFFSETOF(t,f)	((SIZE_T)&(((t *)0)->f))


void DatetimeToString(char* szTimeFormat, int nBuffLen, time_t uTimeStamp)
{
	tm time = { 0 };
	time_t t = uTimeStamp + 28800;
	gmtime_s(&time, &t);
	strftime(szTimeFormat, nBuffLen, "%Y-%m-%d/%H:%M:%S", &time);
}

char* pType[] =
{
	"",          // 0
	"鼠标指针",   // 1
	"位图",   // 2
	"图标",       // 3
	"菜单",       // 4
	"对话框",      //5
	"字符串列表",  //6
	"字体目录",  //7
	"字体",      //8
	"快捷键",  //9
	"非格式化资源",  //A
	"消息列表",  //B
	"鼠标指针组",  //C
	"",             // D
	"图标组",  //E
	"",         // F
	"版本信息"//10
};

//解析资源表
void ParseResource(ULONG_PTR upResRoot, IMAGE_RESOURCE_DIRECTORY* pResDir, int nLevel)
{
	IMAGE_RESOURCE_DIR_STRING_U* pResName;
	wchar_t buff[MAX_PATH];
	switch (nLevel)
	{
	case 1:
	{
			  ULONG ulCount = pResDir->NumberOfIdEntries + pResDir->NumberOfNamedEntries;
			  //遍历所有的资源目录入口
			  IMAGE_RESOURCE_DIRECTORY_ENTRY *pDirEntry = NULL;
			  pDirEntry = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)(pResDir + 1);

			  for (ULONG i = 0; i < ulCount; i++)
			  {
				  if (1 == pDirEntry[i].NameIsString)
				  {
					  pResName = (IMAGE_RESOURCE_DIR_STRING_U*)(pDirEntry[i].NameOffset + upResRoot);
					  memset(buff, 0, sizeof(buff));
					  memcpy(buff, pResName->NameString, pResName->Length);
					  wprintf(L"资源类型:\"%s\"\n", buff);
				  }
				  else
				  {
					  if (pDirEntry[i].Id >= 0 && pDirEntry[i].Id <= 16)
					  {
						  printf("资源类型:\"%s\"\n", pType[pDirEntry[i].Id]);
					  }
					  else
					  {
						  printf("资源类型:[%d]\n", pDirEntry[i].Id);
					  }
				  }
				  // 获取下一层的偏移
				  if (1 == pDirEntry[i].DataIsDirectory) {
					  // 获取下一层目录(第二层目录的入口地址)
					  IMAGE_RESOURCE_DIRECTORY* pNextDir;
					  pNextDir = (IMAGE_RESOURCE_DIRECTORY*)(pDirEntry[i].OffsetToDirectory + upResRoot);
					  ParseResource(upResRoot, pNextDir, 2);
				  }
			  }

	}
		break;
	case 2:
	{
			  ULONG ulCount = pResDir->NumberOfIdEntries + pResDir->NumberOfNamedEntries;
			  IMAGE_RESOURCE_DIRECTORY_ENTRY* pDirEntry;
			  pDirEntry = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)(pResDir + 1);
			  //
			  for (DWORD i = 0; i < ulCount; ++i)
			  {
				  if (1 == pDirEntry[i].NameIsString)
				  {
					  pResName = (IMAGE_RESOURCE_DIR_STRING_U*)(pDirEntry[i].NameOffset + upResRoot);
					  memset(buff, 0, sizeof(buff));
					  memcpy(buff, pResName->NameString, pResName->Length);
					  wprintf(L"|-- 资源ID:\"%s\"\n", buff);
				  }
				  else
				  {
					  printf("|-- 资源ID[%d]\n", pDirEntry[i].Id);
				  }

				  if (1 == pDirEntry[i].DataIsDirectory)
				  {
					  IMAGE_RESOURCE_DIRECTORY* pNextDir;
					  pNextDir = (IMAGE_RESOURCE_DIRECTORY*)(pDirEntry[i].OffsetToDirectory + upResRoot);
					  ParseResource(upResRoot, pNextDir, 3);
				  }
			  }

	}
		break;
	case 3:
	{
			  IMAGE_RESOURCE_DIRECTORY_ENTRY* pDirEntry;
			  pDirEntry = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)(pResDir + 1);
			  if (0 == pDirEntry->DataIsDirectory)
			  {
				  IMAGE_RESOURCE_DATA_ENTRY* pDataEntry;
				  pDataEntry = (IMAGE_RESOURCE_DATA_ENTRY*)(pDirEntry->OffsetToData + upResRoot);
				  printf("|    |--- RVA:%08X,SIZE:%d\n", pDataEntry->OffsetToData, pDataEntry->Size);
			  }

	}
		break;
	default:
		break;
	}
}


int main(int argc, char* argv[])
{
	TCHAR szFilePath[MAX_PATH] = { 0 };
	OPENFILENAME ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = _T("执行文件(*.exe)(*.dll)(*.sys)\0");
	ofn.lpstrInitialDir = _T("./");
	ofn.lpstrFile = szFilePath;
	ofn.nMaxFile = sizeof(szFilePath) / sizeof(*szFilePath);
	ofn.nFilterIndex = 0;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;
	if (!GetOpenFileName(&ofn))
	{
		return 1;
	}
	if (_tcscmp(szFilePath, _T("")) == 0)
	{
		return 1;
	}



	HANDLE hReadFile = CreateFile(szFilePath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hReadFile == INVALID_HANDLE_VALUE) {
		printf("文件不存在\n");
		system("pause");
		return 1;
	}
	ULONG ulHigh = 0;
	ULONG ulFileSize = GetFileSize(hReadFile, &ulHigh);
	BYTE* pFileBuff = new BYTE[ulFileSize];
	ReadFile(hReadFile, pFileBuff, ulFileSize, &ulFileSize, NULL);

	if (!IsPEFile(pFileBuff))
	{
		printf("不是一个pe文件");
		system("pause");
		return 1;
	}

	//获取文件头和扩展头
	IMAGE_NT_HEADERS *pNtHeader = GetNtHeader(pFileBuff);

	char szDateBuff[MAX_PATH] = { 0 };
	DatetimeToString(szDateBuff, _countof(szDateBuff), pNtHeader->FileHeader.TimeDateStamp);
	//打印文件头的信息
	printf("文件头的信息 Machine:%x\n", pNtHeader->FileHeader.Machine);
	printf("文件头的信息 NumberOfSections:%x\n", pNtHeader->FileHeader.NumberOfSections);
	printf("文件头的信息 TimeDateStamp:%s\n", szDateBuff);
	printf("文件头的信息 SizeOfOptionalHeader:%x\n", pNtHeader->FileHeader.SizeOfOptionalHeader);
	printf("文件头的信息 Characteristics:%x\n", pNtHeader->FileHeader.Characteristics);
	printf("\n\n");

	//打印扩展头信息
	printf("扩展头信息 Magic:%x\n", pNtHeader->OptionalHeader.Magic);
	printf("扩展头信息 SizeOfCode:%x\n", pNtHeader->OptionalHeader.SizeOfCode);
	printf("扩展头信息 SizeOfInitializedData:%x\n", pNtHeader->OptionalHeader.SizeOfInitializedData);
	printf("扩展头信息 SizeOfUninitializedData:%x\n", pNtHeader->OptionalHeader.SizeOfUninitializedData);
	printf("扩展头信息 AddressOfEntryPoint:%x\n", pNtHeader->OptionalHeader.AddressOfEntryPoint);
	printf("扩展头信息 BaseOfCode:%x\n", pNtHeader->OptionalHeader.BaseOfCode);
	printf("扩展头信息 ImageBase:%x\n", pNtHeader->OptionalHeader.ImageBase);
	printf("扩展头信息 SectionAlignment:%x\n", pNtHeader->OptionalHeader.SectionAlignment);
	printf("扩展头信息 FileAlignment:%x\n", pNtHeader->OptionalHeader.FileAlignment);
	printf("扩展头信息 SizeOfImage:%x\n", pNtHeader->OptionalHeader.SizeOfImage);
	printf("扩展头信息 SizeOfHeaders:%x\n", pNtHeader->OptionalHeader.SizeOfHeaders);
	printf("扩展头信息 Subsystem:%x\n", pNtHeader->OptionalHeader.Subsystem);
	printf("扩展头信息 DllCharacteristics:%x\n", pNtHeader->OptionalHeader.DllCharacteristics);
	printf("扩展头信息 SizeOfStackReserve:%x\n", pNtHeader->OptionalHeader.SizeOfStackReserve);
	printf("扩展头信息 SizeOfStackCommit:%x\n", pNtHeader->OptionalHeader.SizeOfStackCommit);
	printf("扩展头信息 SizeOfHeapReserve:%x\n", pNtHeader->OptionalHeader.SizeOfHeapReserve);
	printf("扩展头信息 SizeOfHeapCommit:%x\n", pNtHeader->OptionalHeader.SizeOfHeapCommit);
	printf("扩展头信息 NumberOfRvaAndSizes:%x\n", pNtHeader->OptionalHeader.NumberOfRvaAndSizes);
	printf("\n\n");
	//几个特殊的rva转offset
	ULONG ulFileOffset = 0;
	ULONG ulRva = 0;
	RVA2Foa(pFileBuff, pNtHeader->OptionalHeader.BaseOfCode, &ulFileOffset);
	Foa2RVA(pFileBuff, pNtHeader->OptionalHeader.BaseOfCode, &ulRva);

	//数据目录
	IMAGE_DATA_DIRECTORY *pDirectory = GetDirectory(pFileBuff);


	for (ULONG i = 0; i < pNtHeader->OptionalHeader.NumberOfRvaAndSizes; i++)
	{
		printf("数据目录 第%d个表项 Size:%x, VirtualAddress:%x\n", i, pDirectory[i].Size, pDirectory[i].VirtualAddress);
	}
	printf("\n\n");

	//分节信息
	IMAGE_SECTION_HEADER* pSectionHeader = NULL;
	pSectionHeader = IMAGE_FIRST_SECTION(pNtHeader);
	for (ULONG i = 0; i < pNtHeader->FileHeader.NumberOfSections; ++i)
	{
		printf("第%d个分节 Name:%s\n", i, pSectionHeader->Name);
		printf("第%d个分节 VirtualAddress:%x\n", i, pSectionHeader->VirtualAddress);
		printf("第%d个分节 SizeOfRawData:%x\n", i, pSectionHeader->SizeOfRawData);
		printf("第%d个分节 PointerToRawData:%x\n", i, pSectionHeader->PointerToRawData);
		printf("第%d个分节 Characteristics:%x\n", i, pSectionHeader->Characteristics);
		pSectionHeader += 1;
	}


	//获取导出表信息
	IMAGE_EXPORT_DIRECTORY *pExportTable = GetExportTable(pFileBuff);
	if (NULL != pExportTable)
	{
		ULONG ulNameFoa;
		RVA2Foa(pFileBuff, pExportTable->Name, &ulNameFoa);
		printf("导出表 Name:%s\n", (char*)((ULONG_PTR)ulNameFoa + pFileBuff));
		printf("导出表 Base:%d\n", pExportTable->Base);
		printf("导出表 NumberOfFunctions:%d\n", pExportTable->NumberOfFunctions);
		printf("导出表 NumberOfNames:%d\n", pExportTable->NumberOfNames);

		ULONG ulAddressOfFunctionFoa = 0;
		ULONG  ulAddressOfNamesFoa = 0;
		ULONG  ulAddressOfNameOrdinalsFoa = 0;
		RVA2Foa(pFileBuff, pExportTable->AddressOfFunctions, &ulAddressOfFunctionFoa);
		RVA2Foa(pFileBuff, pExportTable->AddressOfNames, &ulAddressOfNamesFoa);
		RVA2Foa(pFileBuff, pExportTable->AddressOfNameOrdinals, &ulAddressOfNameOrdinalsFoa);
		//
		PULONG_PTR upAddressOfFunction = (PULONG_PTR)(pFileBuff + ulAddressOfFunctionFoa);
		PULONG_PTR upAddressOfName = (PULONG_PTR)(pFileBuff + ulAddressOfNamesFoa);
		WORD* upAddressOfNameOrdinals = (WORD*)(pFileBuff + ulAddressOfNameOrdinalsFoa);

		for (ULONG i = 0; i < pExportTable->NumberOfFunctions; i++)
		{
			if (0 == upAddressOfFunction[i]) continue;
			printf("导出表地址:%x\n", upAddressOfFunction[i]);

			ULONG ulIndex = 0;
			for (; ulIndex < pExportTable->NumberOfNames; ++ulIndex)
			{
				if (i == upAddressOfNameOrdinals[ulIndex]) break;
			}

			if (ulIndex == pExportTable->NumberOfNames)
			{
				printf("函数以序号导出:%x\n", i + pExportTable->Base);
				continue;
			}
			ULONG ulNameFoa = 0;
			RVA2Foa(pFileBuff, upAddressOfName[ulIndex], &ulNameFoa);
			char* pFunName = (char*)(pFileBuff + (ULONG_PTR)ulNameFoa);
			printf("函数导出名字为:%s\n", pFunName);
		}
	}
	printf("\n\n");

	//获取导入表信息
	IMAGE_IMPORT_DESCRIPTOR* pImportTable = GetImportTable(pFileBuff);
	if (NULL != pImportTable)
	{
		while (pImportTable->Name != 0)
		{
			ULONG ulNameFoa = 0;
			RVA2Foa(pFileBuff, pImportTable->Name, &ulNameFoa);
			char* szImportDllName = (char*)((ULONG_PTR)pFileBuff + (ULONG_PTR)ulNameFoa);
			printf("导入函数名:%s\n", szImportDllName);

			ULONG ulIntFoa = 0;
			RVA2Foa(pFileBuff, pImportTable->OriginalFirstThunk, &ulIntFoa);
			//获取Int在文件中的地址
			ULONG_PTR* pInt = (ULONG_PTR*)(ulIntFoa + (ULONG_PTR)pFileBuff);
			while (*pInt != 0)
			{
				if (IMAGE_SNAP_BY_ORDINAL(*pInt))
				{
					printf("以序号进行导入, %x\n", *pInt & 0xFFFF);
				}
				else
				{
					IMAGE_IMPORT_BY_NAME* pImpByName = NULL;
					ULONG ulNameFoa = 0;
					RVA2Foa(pFileBuff, *pInt, &ulNameFoa);
					pImpByName = (IMAGE_IMPORT_BY_NAME*)(ulNameFoa + (ULONG_PTR)pFileBuff);
					printf("以名称导入, 序号:%d, 名称:%s\n", pImpByName->Hint, pImpByName->Name);
				}
				++pInt;
			}
			++pImportTable;
		}
	}
	printf("\n\n");
	//获取资源表信息
	IMAGE_RESOURCE_DIRECTORY* pResourceDirectory = GetResourceDirectory(pFileBuff);
	if (NULL != pResourceDirectory)
	{
		ParseResource((ULONG_PTR)pResourceDirectory, pResourceDirectory, 1);
	}

	printf("\n\n");
	//获取重定位表信息
	IMAGE_BASE_RELOCATION *pRelocationTab = GetBaseRelocation(pFileBuff);
	if (NULL != pRelocationTab)
	{
		struct TypeOffset
		{
			WORD Offset : 12;  // (1) 大小为12Bit的重定位偏移
			WORD Type : 4;    // (2) 大小为4Bit的重定位信息类型值
		};

		while (pRelocationTab->SizeOfBlock != 0)
		{

			TypeOffset* pTypeOffset;
			pTypeOffset = (TypeOffset*)(pRelocationTab + 1);
			ULONG ulCount = (pRelocationTab->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);

			for (ULONG i = 0; i < ulCount; ++i)
			{
				if (pTypeOffset[i].Type == 3)
				{
					//printf("RVA:%04X",pRelocationTab->VirtualAddress + pTypeOffset[i].Offset);

					ULONG dwFixAddr = pRelocationTab->VirtualAddress + pTypeOffset[i].Offset;
					ULONG dwFixAddrOffset = 0;
					RVA2Foa(pFileBuff, dwFixAddr, &dwFixAddrOffset);
					printf("需要修改的地址:RVA:0x%08X , OFS: 0x%08X\n", dwFixAddr, dwFixAddrOffset);
				}
			}
			//下个重定位表的位置
			pRelocationTab = (IMAGE_BASE_RELOCATION*)((ULONG_PTR)pRelocationTab + pRelocationTab->SizeOfBlock);
		}
	}

	//延时导入表
	IMAGE_DELAYLOAD_DESCRIPTOR* pDelayLoadDescriptor = GetDelayLoadDescriptor(pFileBuff);
	if (NULL != pDelayLoadDescriptor)
	{
		while (pDelayLoadDescriptor->DllNameRVA != 0)
		{
			ULONG ulNameFoa = 0;
			RVA2Foa(pFileBuff, pDelayLoadDescriptor->DllNameRVA, &ulNameFoa);
			char* szImportDllName = (char*)((ULONG_PTR)pFileBuff + (ULONG_PTR)ulNameFoa);
			printf("延时导入函数名:%s\n", szImportDllName);

			ULONG ulIntFoa = 0;
			RVA2Foa(pFileBuff, pDelayLoadDescriptor->ImportNameTableRVA, &ulIntFoa);
			//获取Int在文件中的地址
			ULONG_PTR* pInt = (ULONG_PTR*)(ulIntFoa + (ULONG_PTR)pFileBuff);
			while (*pInt != 0)
			{
				if (IMAGE_SNAP_BY_ORDINAL(*pInt))
				{
					printf("延时导入 以序号进行导入, %x\n", *pInt & 0xFFFF);
				}
				else
				{
					IMAGE_IMPORT_BY_NAME* pImpByName = NULL;
					ULONG ulNameFoa = 0;
					RVA2Foa(pFileBuff, *pInt, &ulNameFoa);
					pImpByName = (IMAGE_IMPORT_BY_NAME*)(ulNameFoa + (ULONG_PTR)pFileBuff);
					printf("延时导入 以名称导入, 序号:%d, 名称:%s\n", pImpByName->Hint, pImpByName->Name);
				}
				++pInt;
			}

			++pDelayLoadDescriptor;
		}
	}

	//解析tls表
	IMAGE_TLS_DIRECTORY* pTlsDirectory = GetImageTls(pFileBuff);
	if (NULL != pTlsDirectory)
	{
		printf("tls 遍历 StartAddressOfRawData:%x\n", pTlsDirectory->StartAddressOfRawData);
		printf("tls 遍历 EndAddressOfRawData:%x\n", pTlsDirectory->EndAddressOfRawData);
		if (pTlsDirectory->AddressOfCallBacks > 0)
		{
			ULONG uptAdressOfCallBackRva = pTlsDirectory->AddressOfCallBacks - pNtHeader->OptionalHeader.ImageBase;
			ULONG uAdressOfCallBackFoa = 0;
			RVA2Foa(pFileBuff, uptAdressOfCallBackRva, &uAdressOfCallBackFoa);
			ULONG_PTR* uptAdressOfCallBack = (ULONG_PTR*)((ULONG_PTR)pFileBuff + uAdressOfCallBackFoa);

			while (*uptAdressOfCallBack)
			{
				printf("tls 遍历 回调函数地址:%x\n", *uptAdressOfCallBack);
				uptAdressOfCallBack++;
			}
		}
	}

	//解析签名表
	WIN_CERTIFICATE *pWinCertificate = GetCertificate(pFileBuff);
	if (pWinCertificate)
	{
		DWORD dwCertificateSize = pWinCertificate->dwLength - OFFSETOF(WIN_CERTIFICATE, bCertificate);
		PBYTE pTmpBuff = new BYTE[dwCertificateSize];
		memcpy(pTmpBuff, pWinCertificate->bCertificate, dwCertificateSize);
		HANDLE hAnsiFile = CreateFile(_T("d:/Certificate.ansi"),
			GENERIC_ALL,
			FILE_SHARE_READ,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		DWORD dwTmp;
		WriteFile(hAnsiFile, pTmpBuff, dwCertificateSize, &dwTmp, NULL);
		CloseHandle(hAnsiFile);
		delete[] pTmpBuff;
	}

	//关闭读句柄
	CloseHandle(hReadFile);

	//修改imageBase
	ModifyImageBase(pFileBuff, 0x400000);
	ULONG ulRealWrite = 0;
	TCHAR szModifyBaseFile[MAX_PATH] = { 0 };
	_stprintf_s(szModifyBaseFile, _countof(szModifyBaseFile), _T("%s修改ImageBase.exe"), szFilePath);
	HANDLE hModifiImageBase = CreateFile(szModifyBaseFile,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	WriteFile(hModifiImageBase, pFileBuff, ulFileSize, &ulRealWrite, NULL);
	CloseHandle(hModifiImageBase);



	//增加分节
	BYTE pFillBuff[] =
	{
		"\xE8\x00\x00\x00\x00"	// call $+5
		"\x58"					// pop	eax
		"\x6A\x00"				// push	0
		"\x83\xC0\x17"			// add eax,0x17
		"\x50"					// push eax
		"\x83\xC0\x14"			// add eax,0x14
		"\x50"					// push eax
		"\x6A\x00"				// push 0
		"\xB8\x30\x88\x20\x74"	// mov eax,MessageBoxA
		"\xFF\xD0"				// call eax
		"\xC2\x04\x00"			// ret 0x04
		"来自shellcode的问候\0"  //20个字节
		"大家好,我是shellcode!!!" //24个字节
	};

	// 将MessageBoxA函数的地址写入到shellcode中.
	*(DWORD*)(pFillBuff + 19) = (DWORD)&MessageBoxA;
	BYTE* pNewBuffOfAddress; //添加新节后缓冲区
	int nNewFileSize;   //新增加的节缓冲区大小
	IMAGE_SECTION_HEADER newSectionInfo = { 0 }; //新的分节信息
	AddSection(
		pFileBuff,
		ulFileSize,
		"hzh01",
		pFillBuff,
		sizeof(pFillBuff),
		IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE,
		&pNewBuffOfAddress,
		&nNewFileSize, 
		&newSectionInfo);

	IMAGE_NT_HEADERS *pNewNtHeader = GetNtHeader(pNewBuffOfAddress);
	pNewNtHeader->OptionalHeader.AddressOfEntryPoint = newSectionInfo.VirtualAddress;

	TCHAR szAddSectionFile[MAX_PATH] = { 0 };
	_stprintf_s(szAddSectionFile, _countof(szAddSectionFile), _T("%s增加分节.exe"), szFilePath);

	HANDLE hAddSectionFile = CreateFile(szAddSectionFile,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	WriteFile(hAddSectionFile, pNewBuffOfAddress, nNewFileSize, &ulRealWrite, NULL);
	CloseHandle(hAddSectionFile);
	delete[]  pNewBuffOfAddress;
	pNewBuffOfAddress = NULL;

	//把导入表移动到新的分节
	ZeroMemory(&newSectionInfo, sizeof(newSectionInfo));
	MoveImport(
		pFileBuff,
		ulFileSize,
		"myimp",
		IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE,
		&pNewBuffOfAddress,
		&nNewFileSize,
		&newSectionInfo
		);

	//增加导入dll
	AddImportDll(
		pNewBuffOfAddress,
		"002pe结构测试添加导入表的Dll.dll",
		1
		);



	TCHAR szMovImport[MAX_PATH] = { 0 };
	_stprintf_s(szMovImport, _countof(szMovImport), _T("%s移动导入表且修改导入dll.exe"), szFilePath);

	HANDLE hMoveImport = CreateFile(szMovImport,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	WriteFile(hMoveImport, pNewBuffOfAddress, nNewFileSize, &ulRealWrite, NULL);
	CloseHandle(hMoveImport);
	delete[]  pNewBuffOfAddress;
	pNewBuffOfAddress = NULL;


	//收尾工作
	delete[] pFileBuff;

	system("pause");
	return 0;
}
