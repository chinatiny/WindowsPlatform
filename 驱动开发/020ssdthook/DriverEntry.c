#include <ntifs.h>
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

ULONG g_uProtectPID;

typedef NTSTATUS(NTAPI *NTOPENPROCESS)(
	__out PHANDLE  ProcessHandle,
	__in ACCESS_MASK  DesiredAccess,
	__in POBJECT_ATTRIBUTES  ObjectAttributes,
	__in_opt PCLIENT_ID  ClientId
	);
NTOPENPROCESS g_OldOpenProcess = NULL;


NTSTATUS NTAPI MyNtOpenProcess(
	__out PHANDLE  ProcessHandle,
	__in ACCESS_MASK  DesiredAccess,
	__in POBJECT_ATTRIBUTES  ObjectAttributes,
	__in_opt PCLIENT_ID  ClientId
	)
{
	if ((ULONG)ClientId->UniqueProcess == g_uProtectPID)
	{
		return STATUS_ABANDONED;
	}
	return g_OldOpenProcess(
		ProcessHandle,
		DesiredAccess,
		ObjectAttributes,
		ClientId
		);
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
	__asm { //恢复内存保护
		push eax;
		mov eax, cr0;
		or eax, 0x10000;
		mov cr0, eax;
		pop eax;
	}
}
void init()
{
	g_OldOpenProcess =
		(NTOPENPROCESS)KeServiceDescriptorTable.ServiceTableBase[0xBE];
}

void OnHook()
{
	OffProtect();
	KeServiceDescriptorTable.ServiceTableBase[0xBE] = (ULONG)MyNtOpenProcess;
	OnProtect();
}
void OffHook()
{
	OffProtect();
	KeServiceDescriptorTable.ServiceTableBase[0xBE] = (ULONG)g_OldOpenProcess;
	OnProtect();
}





VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	OffHook();
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	g_uProtectPID = 548;
	init();
	OnHook();
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}

