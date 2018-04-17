#include "RegOper.h"

VOID DriverUnload(PDRIVER_OBJECT pDriverObject);

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	pPath;
	pDriver->DriverUnload = DriverUnload;
	DbgBreakPoint();
	EnumSubKeyTest();
	EnumSubValueTest();
	return STATUS_SUCCESS;
}


VOID DriverUnload(PDRIVER_OBJECT pDriverObject)
{
	pDriverObject;
	KdPrint(("driver unload\n"));
}

