#pragma warning(disable:4101 4100 4055 4152 4214)

#include <ntddk.h>  

typedef struct _GDT{
	INT64 Limit0_15 : 16;
	INT64 base0_23 : 24;
	INT64 TYPE : 4;
	INT64 S : 1;
	INT64 DPL : 2;
	INT64 P : 1;
	INT64 Limit16_19 : 4;
	INT64 AVL : 1;
	INT64 O : 1;
	INT64 D_B : 1;
	INT64 G : 1;
	INT64 Base24_31 : 8;
}GDT, *PGDT;//830089f700000068

//SGDT返回的数据格式   
#pragma pack(1)  
typedef struct
{
	UINT16 uGdtLimit;
	UINT16 uLowGdtBase;
	UINT16 uHighGdtBase;
}GDTINFO, *PGDTINFO;
#pragma pack()  


#define MAKE_LONG(Low,High) ((UINT32) (((UINT16)(Low))|((UINT32)((UINT16)(High)))<<16) )

VOID DriverUnload(PDRIVER_OBJECT objDriver)
{
	DbgPrint("My Driver is unloading ...\n");
}
NTSTATUS DriverEntry(
	PDRIVER_OBJECT objDriver,
	PUNICODE_STRING strRegPath
	)
{
	objDriver->DriverUnload = DriverUnload;

	GDTINFO gdtEntry;
	__asm sgdt gdtEntry

	DbgBreakPoint();
	PGDT pGdt = (PGDT)MAKE_LONG(gdtEntry.uLowGdtBase, gdtEntry.uHighGdtBase);
	for (UINT16 i = 0; i < (gdtEntry.uGdtLimit + 1) / sizeof(GDT); i++)
	{
		PGDT  pIndex = &pGdt[i];
		ULONG uAddr = (ULONG)(pIndex->base0_23 | (pIndex->Base24_31 << 23));
		//if (MmIsAddressValid((PVOID)uAddr))
		DbgPrint("index %d: gdtbase: 0x%08X\n", i, uAddr);
	}

	return STATUS_SUCCESS;
}