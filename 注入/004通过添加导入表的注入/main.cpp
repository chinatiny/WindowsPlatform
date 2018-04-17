#include "PEOperation.h"
#include <tchar.h>
#include <stdio.h>

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


	BYTE* pNewBuffOfAddress; //添加新节后缓冲区
	int nNewFileSize;   //新增加的节缓冲区大小
	IMAGE_SECTION_HEADER newSectionInfo = { 0 }; //新的分节信息
	ULONG ulRealWrite;
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
		//"002pe结构测试添加导入表的Dll.dll",
		"AntiCheat.dll",
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

	printf("修改成功，请确保【002pe结构测试添加导入表的Dll.dll】和exe在同一个目录\n");

	system("pause");

	return 0;
}
