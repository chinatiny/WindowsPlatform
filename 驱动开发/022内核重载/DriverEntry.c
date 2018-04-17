#include <ntifs.h>
#include <ntimage.h>


#pragma pack(1)
typedef struct _ServiceDesriptorEntry
{
	ULONG *ServiceTableBase;        // 服务表基址
	ULONG *ServiceCounterTableBase; // 计数表基址
	ULONG NumberOfServices;         // 表中项的个数
	UCHAR *ParamTableBase;          // 参数表基址
}SSDTEntry, *PSSDTEntry;
#pragma pack()
// 导入SSDT
NTSYSAPI SSDTEntry KeServiceDescriptorTable;

PSSDTEntry pNewSSDT;
PCHAR g_pHookpointer;
PCHAR g_pJmpPointer;
VOID DriverUnload(PDRIVER_OBJECT pDriver);
HANDLE KernelCreateFile(
	IN PUNICODE_STRING pstrFile,
	IN BOOLEAN         bIsDir);

ULONG64 KernelGetFileSize(IN HANDLE hfile);
ULONG64 KernelReadFile(
	IN  HANDLE         hfile,
	IN  PLARGE_INTEGER Offset,
	IN  ULONG          ulLength,
	OUT PVOID          pBuffer);
PVOID GetModuleBase(PDRIVER_OBJECT pDriver, PUNICODE_STRING pModuleName);

//读取内核文件到内存，并且将其展开
void GetReloadBuf(PUNICODE_STRING KerPath, PCHAR* pReloadBuf)
{
	LARGE_INTEGER Offset = { 0 };
	//打开文件得到句柄
	HANDLE hFile = KernelCreateFile(KerPath, FALSE);
	//获得文件大小
	ULONG64 uSize = KernelGetFileSize(hFile);
	//申请一块空间
	PCHAR pKernelBuf = (PCHAR)ExAllocatePool(NonPagedPool, uSize);
	//把申请的空间初始化为0
	RtlZeroMemory(pKernelBuf, uSize);
	KernelReadFile(hFile, &Offset, uSize, pKernelBuf);
	//2 展开内核文件
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pKernelBuf;

	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + pKernelBuf);
	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
	*pReloadBuf = ExAllocatePool(NonPagedPool, pNt->OptionalHeader.SizeOfImage);
	RtlZeroMemory(*pReloadBuf, pNt->OptionalHeader.SizeOfImage);
	//2.1 先拷贝PE头部
	RtlCopyMemory(*pReloadBuf, pKernelBuf, pNt->OptionalHeader.SizeOfHeaders);

	//2.2 再拷贝PE各个区段
	for (size_t i = 0; i < pNt->FileHeader.NumberOfSections; i++)
	{
		RtlCopyMemory(
			*pReloadBuf + pSection[i].VirtualAddress,
			pKernelBuf + pSection[i].PointerToRawData,
			pSection[i].SizeOfRawData
			);
	}
	ExFreePool(pKernelBuf);
}


void FixReloc(PCHAR OldKernelBase, PCHAR NewKernelBase)
{
	typedef struct _TYPEOFFSET
	{

		USHORT Offset : 12;
		USHORT type : 4;
	}TYPEOFFSET, *PTYPEOFFSET;
	//1 找到重定位表
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)NewKernelBase;

	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + NewKernelBase);

	PIMAGE_DATA_DIRECTORY pDir = (pNt->OptionalHeader.DataDirectory + 5);

	PIMAGE_BASE_RELOCATION pReloc = (PIMAGE_BASE_RELOCATION)
		(pDir->VirtualAddress + NewKernelBase);
	while (pReloc->SizeOfBlock != 0)
	{
		//2 寻找重定位的位置
		ULONG uCount = (pReloc->SizeOfBlock - 8) / 2;
		PCHAR pSartAddress = (pReloc->VirtualAddress + NewKernelBase);
		PTYPEOFFSET pOffset = (PTYPEOFFSET)(pReloc + 1);
		for (ULONG i = 0; i < uCount; i++)
		{
			if (pOffset->type == 3)
			{
				//3 开始重定位
				//NewBase-DefaultBase = NewReloc-DefaultReloc
				ULONG * pRelocAdd = (ULONG *)(pSartAddress + pOffset->Offset);
				*pRelocAdd += ((ULONG)OldKernelBase - pNt->OptionalHeader.ImageBase);
			}
			pOffset++;
		}

		pReloc = (PIMAGE_BASE_RELOCATION)((PCHAR)pReloc + pReloc->SizeOfBlock);
	}
}

void FixSSDT(PCHAR OldKernelBase, PCHAR NewKernelBase)
{
	//新内核中的某位置 - NewKernelBase = 老内核中的此位置 - OldKernelBase
	//新内核中的某位置  = NewKernelBase-OldKernelBase+老内核中的此位置
	ULONG uOffset = (ULONG)NewKernelBase - (ULONG)OldKernelBase;

	pNewSSDT =
		(PSSDTEntry)((PCHAR)&KeServiceDescriptorTable + uOffset);
	//填充SSDT函数数量
	pNewSSDT->NumberOfServices =
		KeServiceDescriptorTable.NumberOfServices;
	//填充SSDT函数地址表
	pNewSSDT->ServiceTableBase =
		(PULONG)((PCHAR)KeServiceDescriptorTable.ServiceTableBase + uOffset);
	for (ULONG i = 0; i < pNewSSDT->NumberOfServices; i++)
	{
		pNewSSDT->ServiceTableBase[i] = pNewSSDT->ServiceTableBase[i] + uOffset;
	}
	//填充SSDT参数表，表中一个元素占1个字节
	pNewSSDT->ParamTableBase =
		(UCHAR*)((PCHAR)KeServiceDescriptorTable.ParamTableBase + uOffset);

	memcpy(pNewSSDT->ParamTableBase, KeServiceDescriptorTable.ParamTableBase,
		KeServiceDescriptorTable.NumberOfServices);

	//填充SSDT调用次数表,表中一个元素占4个字节
	//pNewSSDT->ServiceCounterTableBase =
	//	(PULONG)((PCHAR)KeServiceDescriptorTable.ServiceCounterTableBase + uOffset);

	//memcpy(pNewSSDT->ServiceCounterTableBase, 
	//	KeServiceDescriptorTable.ServiceCounterTableBase,
	//	KeServiceDescriptorTable.NumberOfServices*4
	//	);

}
//参数1:被搜索的字符串
//参数2:被搜索的字符串长度
//参数3:在哪开始搜
//参数4:参数3的缓冲区大小
void * SearchMemory(char * buf, int BufLenth, char * Mem, int MaxLenth)
{
	int MemIndex = 0;
	int BufIndex = 0;
	for (MemIndex = 0; MemIndex < MaxLenth; MemIndex++)
	{
		BufIndex = 0;
		if (Mem[MemIndex] == buf[BufIndex] || buf[BufIndex] == '?')
		{
			int MemIndexTemp = MemIndex;
			do
			{
				MemIndexTemp++;
				BufIndex++;
			} while ((Mem[MemIndexTemp] == buf[BufIndex] || buf[BufIndex] == '?') && BufIndex < BufLenth);
			if (BufIndex == BufLenth)
			{
				return Mem + MemIndex;
			}
		}
	}
	return 0;
}

PVOID GetKiFastCallEntryAddr()
{
	PVOID pAddr = 0;
	_asm
	{
		push ecx;
		push eax;
		mov ecx, 0x176;
		rdmsr;
		mov pAddr, eax;
		pop eax;
		pop ecx;
	}
	return pAddr;
}
void OffProtect()
{
	__asm { //关闭内存保护
		push eax;
		mov eax, cr0;
		and eax, ~0x10000;
		mov cr0, eax;
		pop eax;
	}

}
void OnProtect()
{
	__asm { //开启内存保护
		push eax;
		mov eax, cr0;
		OR eax, 0x10000;
		mov cr0, eax;
		pop eax;
	}
}
UCHAR CodeBuf[] = { 0x2b, 0xe1, 0xc1, 0xe9, 0x02 };
UCHAR NewCodeBuf[5] = { 0xE9 };

//调用号    函数表基址    函数地址
ULONG  FilterSSDT(ULONG uCallNum, PULONG FunBaseAddress, PULONG FunAdress)
{
	//说明这个调用是用的SSDT，而不是ShowSSDT
	if (FunBaseAddress == KeServiceDescriptorTable.ServiceTableBase)
	{
		if (uCallNum == 190)
		{
			return pNewSSDT->ServiceTableBase[190];
		}
	}
	return (ULONG)FunAdress;
}

_declspec(naked) void MyFilterFunction()
{
	//5 在自己的Hook函数中判断走自己的内核还是原来的内核
	//eax 调用号
	//edi SSDT函数表地址
	//edx 里面是从SSDT表中获得的函数的地址
	_asm
	{
		pushad;
		pushfd;
		push edx;
		push edi;
		push eax;
		call FilterSSDT;
		mov dword ptr ds : [esp + 0x18], eax;
		popfd;
		popad;
		sub     esp, ecx;
		shr     ecx, 2;
		jmp g_pJmpPointer;
	}
}
void OnHookKiFastCall()
{
	//1 先得到KifastcallEntry的地址
	PVOID KiFastCallAdd = GetKiFastCallEntryAddr();
	//2 搜索2b e1 c1 e9 02
	g_pHookpointer = SearchMemory(CodeBuf, 5, KiFastCallAdd, 0x200);
	//3 找到这个位置之后，直接hook
	//3.1关闭页保护
	OffProtect();
	//3.2替换5个字节
	*(ULONG*)(NewCodeBuf + 1) =
		((ULONG)MyFilterFunction - (ULONG)g_pHookpointer - 5);
	memcpy(g_pHookpointer, NewCodeBuf, 5);

	//3.3开启页保护
	OnProtect();
	g_pJmpPointer = g_pHookpointer + 5;
}



NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgBreakPoint();
	PCHAR pNtModuleBase = NULL;
	UNICODE_STRING pNtModuleName;

	//1 找到内核文件，将其展开加载进内存
	PCHAR pReloadBuf = NULL;
	UNICODE_STRING KerPath;
	RtlInitUnicodeString(&KerPath, L"\\??\\C:\\windows\\system32\\ntkrnlpa.exe");

	GetReloadBuf(&KerPath, &pReloadBuf);
	//2 修复重定位ntoskrnl.exe
	RtlInitUnicodeString(&pNtModuleName, L"ntoskrnl.exe");
	pNtModuleBase = (PCHAR)GetModuleBase(pDriver, &pNtModuleName);
	FixReloc(pNtModuleBase, pReloadBuf);
	//3 修复自己的SSDT表
	FixSSDT(pNtModuleBase, pReloadBuf);
	//4 Hook KiFastCallEntry，拦截系统调用
	OnHookKiFastCall();


	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}



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

typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;    //双向链表
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union {
		LIST_ENTRY HashLinks;
		struct {
			PVOID SectionPointer;
			ULONG CheckSum;
		};
	};
	union {
		struct {
			ULONG TimeDateStamp;
		};
		struct {
			PVOID LoadedImports;
		};
	};
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

PVOID GetModuleBase(PDRIVER_OBJECT pDriver, PUNICODE_STRING pModuleName)
{
	PLDR_DATA_TABLE_ENTRY pLdr =
		(PLDR_DATA_TABLE_ENTRY)pDriver->DriverSection;
	LIST_ENTRY *pTemp = &pLdr->InLoadOrderLinks;
	do
	{
		PLDR_DATA_TABLE_ENTRY pDriverInfo =
			(PLDR_DATA_TABLE_ENTRY)pTemp;
		KdPrint(("%wZ\n", &pDriverInfo->FullDllName));
		if (
			RtlCompareUnicodeString(pModuleName, &pDriverInfo->BaseDllName, FALSE)
			== 0)
		{
			return pDriverInfo->DllBase;
		}
		pTemp = pTemp->Blink;
	} while (pTemp != &pLdr->InLoadOrderLinks);
	return 0;
}




VOID DriverUnload(PDRIVER_OBJECT pDriver)
{

	UNREFERENCED_PARAMETER(pDriver);
}