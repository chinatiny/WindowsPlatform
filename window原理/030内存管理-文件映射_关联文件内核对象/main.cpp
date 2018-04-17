// 内存管理_文件映射.cpp : 定义控制台应用程序的入口点。
//

#include <tchar.h>
#include <Windows.h>
#include <stdio.h>


int _tmain(int argc, _TCHAR* argv[])
{
	// 文件映射步骤:
	// 1. 打开文件(内核对象)
	// 2. 创建文件映射对象(内核对象)
	// 3. 将文件映射对象映射到虚拟内存.
	HANDLE hFile = 0;
	// 1. 打开文件.
	hFile = CreateFile(L"test_mapfile.txt",
		GENERIC_ALL,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (hFile == INVALID_HANDLE_VALUE) {
		printf("文件不存在\n");
		return 0;
	}

	DWORD   dwHigSize = 0;
	DWORD	dwLowSize = GetFileSize(hFile, &dwHigSize);
	// 2. 创建文件映射对象
	HANDLE hMapping = CreateFileMapping(hFile,		/* 被映射的文件对象 */
		NULL,				/* 安全描述符 */
		PAGE_READWRITE,/* 映射到内存后的内存分页属性 */
		dwHigSize,		/* 映射的大小的高32位 */
		dwLowSize,		/* 映射大小的低32位 */
		NULL	/* 文件映射对象的全局名字 */
		);
	if (hMapping == NULL) {
		printf("创建文件映射对象失败\n");
		CloseHandle(hFile);
		return 0;
	}

	// 3. 将文件映射对象映射到内存.
	LPVOID	pBuff = NULL;
	pBuff = MapViewOfFile(hMapping,	 /* 文件映射对象 */
		FILE_MAP_ALL_ACCESS,	/* 权限 */
		0,			 /* 映射到内存的文件偏移(低32位) */
		0,			 /* 映射到内存的文件偏移(高32位) */
		50			 /* 映射到内存的字节数 */
		);
	if (pBuff == NULL) {
		printf("将文件内容映射到内存时失败\n");
		// 关闭文件映射
		CloseHandle(hMapping);
		CloseHandle(hFile);
	}



	// 修改内存, 同时也会影响文件
	strcpy_s((char*)pBuff, 256, "Hello World\r\n");

	FlushViewOfFile(pBuff, 20);
	// 取消映射
	UnmapViewOfFile(pBuff);

	// 关闭文件映射
	CloseHandle(hMapping);

	// 关闭文件
	CloseHandle(hFile);

	return 0;
}
