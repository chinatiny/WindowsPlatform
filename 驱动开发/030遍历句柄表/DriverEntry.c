#include "DriverEntry.h"


NTSTATUS EnumTable1(ULONG uTableCode)
{
	ULONG               uIndex = 1;
	PHANDLE_TABLE_ENTRY pHandleTableEntry = NULL;

	pHandleTableEntry = (PHANDLE_TABLE_ENTRY)((ULONG)(*(PULONG)uTableCode) + 8);
	for (uIndex = 1; uIndex < 0x200; uIndex++)
	{
		if (pHandleTableEntry->Object != NULL)
		{
			PULONG pObjAddr = (PULONG)((SIZE_T)pHandleTableEntry->Object & 0xFFFFFFF8);
			POBJECT_HEADER pHeader = OBJECT_TO_OBJECT_HEADER(pObjAddr);

			//7 ÊÇprocess, 8ÊÇthread
			if (MmIsAddressValid(pHeader))
			{
				KdPrint(("object type is:%d\n", pHeader->TypeIndex));
			}
		}
		pHandleTableEntry++;
	}
	return STATUS_UNSUCCESSFUL;
}

NTSTATUS EnumTable2(ULONG uTableCode)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	do
	{
		status = EnumTable1(uTableCode);
		if (NT_SUCCESS(status))
		{
			return status;
		}
		uTableCode += 4;
	} while (*(PULONG)uTableCode != 0);
	return status;
}

NTSTATUS EnumTable3(ULONG uTableCode)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	do
	{
		status = EnumTable2(uTableCode);
		if (NT_SUCCESS(status))
		{
			return status;
		}
		uTableCode += 4;
	} while (*(PULONG)uTableCode != 0);
	return status;
}

ULONG GetPspCidTableValue()
{
	PVOID           pPsLookupProcessByProcessIdAddress = NULL;
	ULONG           ulPspCidTableValue = 0;
	ULONG           uIndex = 0;
	UNICODE_STRING  ustrFuncName;

	RtlInitUnicodeString(&ustrFuncName, L"PsLookupProcessByProcessId");
	pPsLookupProcessByProcessIdAddress = MmGetSystemRoutineAddress(&ustrFuncName);
	if (pPsLookupProcessByProcessIdAddress == NULL)
	{
		return ulPspCidTableValue;
	}
	KdPrint(("PsLookupProcessByProcessId->%08X", pPsLookupProcessByProcessIdAddress));
	for (uIndex = 0; uIndex < 0x1000; uIndex++)
	{
		if (*((PUCHAR)((ULONG)pPsLookupProcessByProcessIdAddress + uIndex)) == 0x8B &&
			*((PUCHAR)((ULONG)pPsLookupProcessByProcessIdAddress + uIndex + 1)) == 0x3D &&
			*((PUCHAR)((ULONG)pPsLookupProcessByProcessIdAddress + uIndex + 6)) == 0xE8)
		{
			KdPrint(("Found PspCidTable OK!!"));
			ulPspCidTableValue = *((PULONG)((ULONG)pPsLookupProcessByProcessIdAddress + uIndex + 2));
			break;
		}
	}
	return ulPspCidTableValue;
}

NTSTATUS EnumPspCidTable()
{
	ULONG           uFlag = 0;
	ULONG           uTableCode = 0;
	ULONG           ulPspCidTable = 0;
	NTSTATUS        status = STATUS_SUCCESS;
	PHANDLE_TABLE   pHandleTable = NULL;

	ulPspCidTable = GetPspCidTableValue();
	if (ulPspCidTable == 0)
	{
		return STATUS_UNSUCCESSFUL;
	}

	pHandleTable = (PHANDLE_TABLE)(*(PULONG)ulPspCidTable);
	KdPrint(("pHandleTable->%p", pHandleTable));

	uTableCode = (pHandleTable->TableCode) & 0xFFFFFFFC;
	uFlag = (pHandleTable->TableCode) & 0x03;
	KdPrint(("uTableCode->%08X", uTableCode));

	switch (uFlag)
	{
	case 0:
	{
			  status = EnumTable1(uTableCode);
			  break;
	}
	case 1:
	{
			  status = EnumTable2(uTableCode);
			  break;
	}
	case 2:
	{
			  status = EnumTable3(uTableCode);
			  break;
	}
	}
	return status;
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
	DbgBreakPoint();
	DriverObject->DriverUnload = UnloadDriver;
	EnumPspCidTable();
	return STATUS_SUCCESS;
}
