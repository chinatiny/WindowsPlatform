/*
	在增加句柄表引用的时候会调用内核hook相关的函数,具体看文档
*/

#include <ntifs.h>

#pragma warning(disable:4055)

VOID DriverUnload(PDRIVER_OBJECT objDriver);

typedef ULONG(*OBGETOBJECTTYPE)(PVOID Object);

typedef struct _OBJECT_TYPE_INITIALIZER {
	USHORT Length;
	UCHAR ObjectTypeFlags;
	UCHAR CaseInsensitive;
	UCHAR UnnamedObjectsOnly;
	UCHAR  UseDefaultObject;
	UCHAR  SecurityRequired;
	UCHAR MaintainHandleCount;
	UCHAR MaintainTypeList;
	UCHAR SupportsObjectCallbacks;
	UCHAR CacheAligned;
	ULONG ObjectTypeCode;
	BOOLEAN InvalidAttributes;
	GENERIC_MAPPING GenericMapping;
	BOOLEAN   ValidAccessMask;
	BOOLEAN   RetainAccess;
	POOL_TYPE PoolType;
	BOOLEAN DefaultPagedPoolCharge;
	BOOLEAN DefaultNonPagedPoolCharge;
	PVOID DumpProcedure;
	ULONG OpenProcedure;
	PVOID CloseProcedure;
	PVOID DeleteProcedure;
	ULONG ParseProcedure;
	ULONG SecurityProcedure;
	ULONG QueryNameProcedure;
	UCHAR OkayToCloseProcedure;
} OBJECT_TYPE_INITIALIZER, *POBJECT_TYPE_INITIALIZER;
typedef struct _OBJECT_TYPE {
	LIST_ENTRY TypeList;
	UNICODE_STRING Name;
	PVOID DefaultObject;
	ULONG Index;
	ULONG TotalNumberOfObjects;
	ULONG TotalNumberOfHandles;
	ULONG HighWaterNumberOfObjects;
	ULONG HighWaterNumberOfHandles;
	OBJECT_TYPE_INITIALIZER TypeInfo;
	ULONG  TypeLock;
	ULONG   Key;
	LIST_ENTRY   CallbackList;
} OBJECT_TYPE, *POBJECT_TYPE;

typedef NTSTATUS(*PARSEPRODECEDURE)(
	IN PVOID ParseObject,
	IN PVOID ObjectType,
	IN OUT PACCESS_STATE AccessState,
	IN KPROCESSOR_MODE AccessMode,
	IN ULONG Attributes,
	IN OUT PUNICODE_STRING CompleteName,
	IN OUT PUNICODE_STRING RemainingName,
	IN OUT PVOID Context OPTIONAL,
	IN PSECURITY_QUALITY_OF_SERVICE SecurityQos OPTIONAL,
	OUT PVOID *Object);

PARSEPRODECEDURE g_OldFun;



NTSTATUS NewParseProcedure(IN PVOID ParseObject,
	IN PVOID ObjectType,
	IN OUT PACCESS_STATE AccessState,
	IN KPROCESSOR_MODE AccessMode,
	IN ULONG Attributes,
	IN OUT PUNICODE_STRING CompleteName,
	IN OUT PUNICODE_STRING RemainingName,
	IN OUT PVOID Context OPTIONAL,
	IN PSECURITY_QUALITY_OF_SERVICE SecurityQos OPTIONAL,
	OUT PVOID *Object)
{
	KdPrint(("Hook Success"));
	return g_OldFun(ParseObject, ObjectType, AccessState, AccessMode, Attributes,
		CompleteName,
		RemainingName, Context, SecurityQos, Object);
}

OBGETOBJECTTYPE g_OBGetObjectType;
//获得一个函数地址，这个函数能够通过内核对象得到内核对象的类型

VOID GetObGetObjectTypeAddress()
{
	OBGETOBJECTTYPE addr;
	UNICODE_STRING pslookup;
	RtlInitUnicodeString(&pslookup, L"ObGetObjectType");
	addr = (OBGETOBJECTTYPE)MmGetSystemRoutineAddress(&pslookup);//根据名称得到函数地址
	g_OBGetObjectType = (OBGETOBJECTTYPE)addr;
}


OBJECT_TYPE * g_FileType = NULL;
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


void OnHook()
{
	//1 随便打开一个文件，得到一个文件对象
	UNICODE_STRING ustrFilePath;
	RtlInitUnicodeString(&ustrFilePath,
		L"\\??\\D:\\123.txt");
	HANDLE hFile = KernelCreateFile(&ustrFilePath, FALSE);
	PVOID pObject;

	ObReferenceObjectByHandle(hFile, GENERIC_ALL, NULL, KernelMode, &pObject, NULL);

	//2 通过这个文件对象得到OBJECT_TYPE这个结构体
	g_FileType = (OBJECT_TYPE *)g_OBGetObjectType(pObject);

	//3 把这个函数地址保存起来
	g_OldFun = (PARSEPRODECEDURE)g_FileType->TypeInfo.ParseProcedure;

	//4 把这个函数地址替换为自己的函数。
	g_FileType->TypeInfo.ParseProcedure = (ULONG)NewParseProcedure;
}


NTSTATUS DriverEntry(
	PDRIVER_OBJECT  pDriver,
	PUNICODE_STRING strRegPath)
{
	// 避免编译器报未引用参数的警告
	UNREFERENCED_PARAMETER(strRegPath);
	// 打印一行字符串，并注册驱动卸载函数，以便于驱动卸载
	DbgBreakPoint();//_asm int 3
	__try
	{
		GetObGetObjectTypeAddress();
		OnHook();
	}
	except(1)
	{
		KdPrint(("掠过一个异常\n"));
	}
	KdPrint(("My First Dirver!"));
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}


VOID DriverUnload(PDRIVER_OBJECT objDriver) {
	// 避免编译器报未引用参数的警告
	UNREFERENCED_PARAMETER(objDriver);
	// 什么也不做，只打印一行字符串
	KdPrint(("My Dirver is unloading..."));
	g_FileType->TypeInfo.ParseProcedure = (ULONG)g_OldFun;
}