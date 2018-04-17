#include <ntifs.h>


VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	KdPrint(("Leave"));
}
PEPROCESS LookupProcess(HANDLE hPid)
{
	PEPROCESS pEProcess = NULL;
	if (NT_SUCCESS(PsLookupProcessByProcessId(
		hPid, &pEProcess)))
		return pEProcess;
	return NULL;
}

VOID EnumModuleList(PEPROCESS Process)
{
	KAPC_STATE ks;
	//获取PEB地址
	//依附进程
	KeStackAttachProcess(Process, &ks);
	char *buf = (char*)0x29f7c8;
	KdPrint(("%s", buf));
	DbgBreakPoint();
	//取消依附进程
	KeUnstackDetachProcess(&ks);
}



NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	KdPrint(("Hello 15PB\n"));

	DbgBreakPoint();
	PEPROCESS pEprocess = LookupProcess((HANDLE)3700);
	EnumModuleList(pEprocess);


	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}
