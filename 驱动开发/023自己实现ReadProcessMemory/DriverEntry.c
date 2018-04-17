#include <Ntifs.h>
#pragma warning(disable:4189 4100)

#pragma pack(1)
typedef struct _ServiceDesriptorEntry
{
	ULONG *ServiceTableBase;        // 服务表基址
	ULONG *ServiceCounterTableBase; // 计数表基址
	ULONG NumberOfServices;         // 表中项的个数
	UCHAR *ParamTableBase;          // 参数表基址
}SSDTEntry, *PSSDTEntry;
#pragma pack()

//旧的ssdt表的数据
ULONG *gOldServiceTableBase = NULL;
UCHAR *gOldParamTableBase = NULL;



// 申明SSDT
NTSYSAPI SSDTEntry KeServiceDescriptorTable;
VOID DriverUnload(PDRIVER_OBJECT pDriver);
NTSTATUS MyNtReadVirtualMemory(ULONG uFromPid, PVOID  pFromAddress, PVOID pToBuff, ULONG uReadSize);
#pragma  alloc_text(NONE_PAGE, MyNtReadVirtualMemory)


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING path)
{
	DbgBreakPoint();
	pDriver->DriverUnload = DriverUnload;
	//在ssdt表中新增自己的函数
	PULONG pNewServiceTableBase = ExAllocatePool(NonPagedPool, (KeServiceDescriptorTable.NumberOfServices  + 1)* sizeof(ULONG));
	PUCHAR pNewParamTableBase = ExAllocatePool(NonPagedPool, (KeServiceDescriptorTable.NumberOfServices +1)* sizeof(UCHAR));
	RtlCopyMemory(pNewServiceTableBase, KeServiceDescriptorTable.ServiceTableBase, (KeServiceDescriptorTable.NumberOfServices + 1)* sizeof(ULONG));
	RtlCopyMemory(pNewParamTableBase, KeServiceDescriptorTable.ParamTableBase, (KeServiceDescriptorTable.NumberOfServices + 1)* sizeof(UCHAR));
	__asm cli
	ULONG uIndex = KeServiceDescriptorTable.NumberOfServices;
	//备份一下
	gOldServiceTableBase = KeServiceDescriptorTable.ServiceTableBase;
	gOldParamTableBase = KeServiceDescriptorTable.ParamTableBase;
	//设置为新的
	KeServiceDescriptorTable.ServiceTableBase = pNewServiceTableBase;
	KeServiceDescriptorTable.ParamTableBase = pNewParamTableBase;
	KeServiceDescriptorTable.ServiceTableBase[uIndex] = (ULONG)MyNtReadVirtualMemory;
	KeServiceDescriptorTable.ParamTableBase[uIndex] = 0x10;
	KeServiceDescriptorTable.NumberOfServices++;
	__asm sti


	return STATUS_SUCCESS;
}

NTSTATUS MyNtReadVirtualMemory(ULONG uFromPid, PVOID  pFromAddress, PVOID pToBuff, ULONG uReadSize)
{
	NTSTATUS status = STATUS_SUCCESS;

	//获取目标进程信息
	PEPROCESS pFromEProcess = NULL;
	status = PsLookupProcessByProcessId((HANDLE)uFromPid, &pFromEProcess);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("没法根据传入的进程id找到对应的eprocess\n"));
		return status;
	}

	//对输出缓冲区做检查
	if (!MmIsAddressValid(pToBuff))
	{
		KdPrint(("传入的pToBuff导致pagefault\n"));
		ObDereferenceObject(pFromEProcess);
		return STATUS_ABANDONED;
	}

	//进程挂靠
	KAPC_STATE ks;
	KeStackAttachProcess(pFromEProcess, &ks);

	//对读取缓冲区做检查
	if (!MmIsAddressValid(pFromAddress))
	{
		KdPrint(("传入的pFromAddress导致pagefault\n"));
		KeUnstackDetachProcess(&ks);
		ObDereferenceObject(pFromEProcess);
		return STATUS_ABANDONED;
	}
	//申请临时空间
	PVOID pTmbBuff = ExAllocatePool(NonPagedPool, uReadSize);

	//拷贝到系统空间
	__try
	{
		RtlCopyMemory(pTmbBuff, pFromAddress, uReadSize);
	}
	__except (1)
	{
		KdPrint(("从pFromAddress读取数据错误\n"));
		ExFreePool(pTmbBuff);
		KeUnstackDetachProcess(&ks);
		ObDereferenceObject(pFromEProcess);
		return STATUS_ABANDONED;
	}
	KeUnstackDetachProcess(&ks);

	//从临时缓冲区写到目标
	__try
	{
		RtlCopyMemory(pToBuff, pTmbBuff, uReadSize);
	}
	__except (1)
	{
		KdPrint(("从临时缓冲区拷贝到目标缓冲区错误\n"));
		ExFreePool(pTmbBuff);
		KeUnstackDetachProcess(&ks);
		ObDereferenceObject(pFromEProcess);
		return STATUS_ABANDONED;
	}

	//清理
	ExFreePool(pTmbBuff);
	ObDereferenceObject(pFromEProcess);
	return status;
}

VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	//恢复ssdt表
	PUCHAR pNewParamTableBase = KeServiceDescriptorTable.ParamTableBase;
	PULONG pNewServiceTableBase = KeServiceDescriptorTable.ServiceTableBase;
	__asm cli
	KeServiceDescriptorTable.ServiceTableBase = gOldServiceTableBase;
	KeServiceDescriptorTable.ParamTableBase = gOldParamTableBase;
	KeServiceDescriptorTable.NumberOfServices--;
	__asm sti
	ExFreePool(pNewParamTableBase);
	ExFreePool(pNewServiceTableBase);
	KdPrint(("恢复ssdt表\n"));
}