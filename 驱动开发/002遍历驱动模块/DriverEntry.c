#include <ntddk.h>
typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;    //Ë«ÏòÁ´±í
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
		struct _no_name_struct{
			PVOID SectionPointer;
			ULONG CheckSum;
		}_no_name_struct_1;
	} _no_name_union_1;
	union {
		struct {
			ULONG TimeDateStamp;
		}_no_name_struct_2;
		struct {
			PVOID LoadedImports;
		}_no_name_struct_3;
	}_no_name_union_2;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;


VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	pDriver;
	KdPrint(("Driver Unload"));
}

void EnumDirver(PDRIVER_OBJECT pDriver)
{
	PLDR_DATA_TABLE_ENTRY pLdr =
		(PLDR_DATA_TABLE_ENTRY)pDriver->DriverSection;
	LIST_ENTRY *pTemp = &pLdr->InLoadOrderLinks;
	do
	{
		PLDR_DATA_TABLE_ENTRY pDriverInfo =(PLDR_DATA_TABLE_ENTRY)pTemp;
		KdPrint(("%wZ\n", &pDriverInfo->FullDllName));
		pTemp = pTemp->Blink;
	} while (pTemp != &pLdr->InLoadOrderLinks);
}
NTSTATUS DriverEntry(
	PDRIVER_OBJECT pDriver,
	PUNICODE_STRING pPath
	)
{
	pPath;
	_asm int 3;
	EnumDirver(pDriver);
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}

