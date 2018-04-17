#include<ntifs.h>
/*函数声明*/
#define _countof(a)  sizeof(a)/sizeof(a[0])


VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	KdPrint(("Bye Bye\n"));
}

//1 创建文件
HANDLE KernelCreateFile(
	IN PUNICODE_STRING pstrFile, // 文件路径符号链接
	IN BOOLEAN         bIsDir)   // 是否为文件夹
{
	HANDLE          hFile = NULL;
	NTSTATUS        Status = STATUS_UNSUCCESSFUL;
	IO_STATUS_BLOCK StatusBlock = { 0 };
	ULONG           ulShareAccess =
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
	ULONG           ulCreateOpt =
		FILE_SYNCHRONOUS_IO_NONALERT;
	// 1. 初始化OBJECT_ATTRIBUTES的内容
	OBJECT_ATTRIBUTES objAttrib = { 0 };
	ULONG             ulAttributes =
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
	InitializeObjectAttributes(
		&objAttrib,    // 返回初始化完毕的结构体
		pstrFile,      // 文件对象名称
		ulAttributes,  // 对象属性
		NULL, NULL);   // 一般为NULL

	// 2. 创建文件对象
	ulCreateOpt |= bIsDir ?
	FILE_DIRECTORY_FILE : FILE_NON_DIRECTORY_FILE;
	Status = ZwCreateFile(
		&hFile,                // 返回文件句柄
		GENERIC_ALL,           // 文件操作描述
		&objAttrib,            // OBJECT_ATTRIBUTES
		&StatusBlock,          // 接受函数的操作结果
		0,                     // 初始文件大小
		FILE_ATTRIBUTE_NORMAL, // 新建文件的属性
		ulShareAccess,         // 文件共享方式
		FILE_OPEN_IF,          // 文件存在则打开不存在则创建
		ulCreateOpt,           // 打开操作的附加标志位
		NULL,                  // 扩展属性区
		0);                   // 扩展属性区长度
	if (!NT_SUCCESS(Status))
		return (HANDLE)-1;
	return hFile;
}

//2 获得文件大小
ULONG64 KernelGetFileSize(IN HANDLE hfile)
{
	// 查询文件状态
	IO_STATUS_BLOCK           StatusBlock = { 0 };
	FILE_STANDARD_INFORMATION fsi = { 0 };
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	Status = ZwQueryInformationFile(
		hfile,        // 文件句柄
		&StatusBlock, // 接受函数的操作结果
		&fsi,         // 根据最后一个参数的类型输出相关信息
		sizeof(FILE_STANDARD_INFORMATION),
		FileStandardInformation);
	if (!NT_SUCCESS(Status))
		return 0;
	return fsi.EndOfFile.QuadPart;
}

//3 读取文件
ULONG64 KernelReadFile(
	IN  HANDLE         hfile,    // 文件句柄
	IN  PLARGE_INTEGER Offset,   // 从哪里开始读取
	IN  ULONG          ulLength, // 读取多少字节
	OUT PVOID          pBuffer)  // 保存数据的缓存
{
	// 1. 读取文件
	IO_STATUS_BLOCK StatusBlock = { 0 };
	NTSTATUS        Status = STATUS_UNSUCCESSFUL;
	Status = ZwReadFile(
		hfile,        // 文件句柄
		NULL,         // 信号状态(一般为NULL)
		NULL, NULL,   // 保留
		&StatusBlock, // 接受函数的操作结果
		pBuffer,      // 保存读取数据的缓存
		ulLength,     // 想要读取的长度
		Offset,       // 读取的起始偏移
		NULL);        // 一般为NULL
	if (!NT_SUCCESS(Status))  return 0;
	// 2. 返回实际读取的长度
	return StatusBlock.Information;
}

//4 写入文件
ULONG64 KernelWriteFile(
	IN HANDLE         hfile,    // 文件句柄
	IN PLARGE_INTEGER Offset,   // 从哪里开始写入
	IN ULONG          ulLength, // 写入多少字节
	IN PVOID          pBuffer)  // 欲写入的数据
{
	// 1. 写入文件
	IO_STATUS_BLOCK StatusBlock = { 0 };
	NTSTATUS        Status = STATUS_UNSUCCESSFUL;
	Status = ZwWriteFile(
		hfile,        // 文件句柄
		NULL,         // 信号状态(一般为NULL)
		NULL, NULL,   // 保留
		&StatusBlock, // 接受函数的操作结果
		pBuffer,      // 欲写入的数据
		ulLength,     // 想要写入的长度
		Offset,       // 写入的起始偏移
		NULL);        // 一般为NULL
	if (!NT_SUCCESS(Status))  return 0;
	// 2. 返回实际写入的长度
	return StatusBlock.Information;
}

//5 删除文件
NTSTATUS KernelDeleteFile(IN PUNICODE_STRING pstrFile)
{
	// 1. 初始化OBJECT_ATTRIBUTES的内容
	OBJECT_ATTRIBUTES objAttrib = { 0 };
	ULONG             ulAttributes =
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
	InitializeObjectAttributes(
		&objAttrib,    // 返回初始化完毕的结构体
		pstrFile,      // 文件对象名称
		ulAttributes,  // 对象属性
		NULL,          // 根目录(一般为NULL)
		NULL);         // 安全属性(一般为NULL)
	// 2. 删除指定文件/文件夹
	return ZwDeleteFile(&objAttrib);
}


BOOLEAN KernelFindFirstFile(
	IN  HANDLE                     hFile, // 文件句柄
	IN  ULONG                      ulLen, // 信息长度
	OUT PFILE_BOTH_DIR_INFORMATION  pDir, // 文件信息
	IN  ULONG                      uFirstlLen, // 信息长度
	OUT PFILE_BOTH_DIR_INFORMATION pFirstDir // 第一个文件信息
	){
	NTSTATUS                   Status = STATUS_UNSUCCESSFUL;
	IO_STATUS_BLOCK            StatusBlock = { 0 };
	// 1. 获取第一个文件信息，看是否成功
	Status = ZwQueryDirectoryFile(
		hFile, NULL, NULL, NULL,// 文件句柄
		&StatusBlock, // 接受函数的操作结果
		pFirstDir,         // 文件信息
		uFirstlLen,        // “文件信息“的数据长度
		FileBothDirectoryInformation, // 查询模式
		TRUE,              // 是否返回一条起始信息
		NULL,              // 文件句柄指向的文件(一般为NULL)
		FALSE);            // 是否从目录开始的第一项扫描
	// 2. 若成功，则获取文件列表
	if (NT_SUCCESS(Status) == FALSE){
		return FALSE;
	}
	Status = ZwQueryDirectoryFile(
		hFile, NULL, NULL, NULL,// 文件句柄
		&StatusBlock, // 接受函数的操作结果
		pDir,         // 文件信息
		ulLen,        // “文件信息“的数据长度
		FileBothDirectoryInformation, // 查询模式
		FALSE,        // 是否返回一条起始信息
		NULL,         // 文件句柄指向的文件(一般为NULL)
		FALSE);       // 是否从目录开始的第一项扫描
	return NT_SUCCESS(Status);
}


BOOLEAN KernelFindNextFile(
	IN  PFILE_BOTH_DIR_INFORMATION pDirList,//
	OUT PFILE_BOTH_DIR_INFORMATION pDirInfo,
	IN OUT LONG * Loc){
	// 如果有下一项，则移动指针指向下一项
	PFILE_BOTH_DIR_INFORMATION pDir = (PFILE_BOTH_DIR_INFORMATION)((PCHAR)pDirList + *Loc);
	LONG StructLenth = 0;
	if (pDir->FileName[0] != 0)
	{
		StructLenth = sizeof(FILE_BOTH_DIR_INFORMATION);
		memcpy(pDirInfo, pDir, StructLenth + pDir->FileNameLength);
		*Loc = *Loc + pDir->NextEntryOffset;
		if (pDir->NextEntryOffset == 0){
			*Loc = *Loc + StructLenth + pDir->FileNameLength;
		}
		return TRUE;
	}
	return FALSE;
}
NTSTATUS EnumFile()
{
	UNICODE_STRING ustrFolder = { 0 };
	WCHAR          szSymbol[0x512] = L"\\??\\";
	UNICODE_STRING ustrPath =
		RTL_CONSTANT_STRING(L"C:\\");
	HANDLE         hFile = NULL;
	SIZE_T         nFileInfoSize =
		sizeof(FILE_BOTH_DIR_INFORMATION)+270 * sizeof(WCHAR);
	SIZE_T         nSize = nFileInfoSize * 0x256; //假设最多有0x256个文件
	char           strFileName[0x256] = { 0 };
	PFILE_BOTH_DIR_INFORMATION pFileTemp = NULL;
	PFILE_BOTH_DIR_INFORMATION pFileList = NULL;
	pFileList = (PFILE_BOTH_DIR_INFORMATION)ExAllocatePool(PagedPool, nSize);
	pFileTemp = (PFILE_BOTH_DIR_INFORMATION)ExAllocatePool(PagedPool,
		nFileInfoSize);
	// 1. 将路径组装为连接符号名，并打开文件
	wcscat_s(szSymbol, _countof(szSymbol), ustrPath.Buffer);
	RtlInitUnicodeString(&ustrFolder, szSymbol);
	hFile = KernelCreateFile(&ustrFolder, TRUE);
	if (KernelFindFirstFile(hFile, nSize, pFileList, nFileInfoSize, pFileTemp))
	{
		LONG Loc = 0;
		do
		{
			RtlZeroMemory(strFileName, 0x256);
			RtlCopyMemory(strFileName,
				pFileTemp->FileName,
				pFileTemp->FileNameLength);
			if (strcmp(strFileName, "..") == 0
				|| strcmp(strFileName, ".") == 0)
				continue;
			if (pFileTemp->FileAttributes
				& FILE_ATTRIBUTE_DIRECTORY)
				DbgPrint("[目录]%S\n", strFileName);
			else
				DbgPrint("[文件]%S\n", strFileName);
			memset(pFileTemp, 0, nFileInfoSize);
		} while (KernelFindNextFile(pFileList, pFileTemp, &Loc));
	}
	return STATUS_SUCCESS;
}
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	KdPrint(("Hello 15PB\n"));
	DbgBreakPoint();
	EnumFile();
	pDriver->DriverUnload = DriverUnload;

	return STATUS_SUCCESS;
}

