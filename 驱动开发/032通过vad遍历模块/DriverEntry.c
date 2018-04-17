#pragma warning(disable:4189 4100 4201 4214)
#include <ntifs.h>

typedef struct _MMADDRESS_NODE // 0x14
{
	ULONG u1; // +0x0(0x4)
	struct _MMADDRESS_NODE* LeftChild; // +0x4(0x4)
	struct _MMADDRESS_NODE* RightChild; // +0x8(0x4)
	ULONG StartingVpn; // +0xc(0x4)
	ULONG EndingVpn; // +0x10(0x4)
}MMADDRESS_NODE, *PMMADDRESS_NODE;

#pragma pack(push,1)
typedef struct _EX_FAST_REF
{
	union
	{
		PVOID Object;
		ULONG_PTR RefCnt : 3;
		ULONG_PTR Value;
	};
} EX_FAST_REF, *PEX_FAST_REF;
#pragma pack(pop)

struct _SEGMENT // 0x38
{
	struct _CONTROL_AREA* ControlArea; // +0x0(0x4)
	ULONG TotalNumberOfPtes; // +0x4(0x4)
	ULONG SegmentFlags; // +0x8(0x4)
	ULONG NumberOfCommittedPages; // +0xc(0x4)
	ULONGLONG SizeOfSegment; // +0x10(0x8)
	union
	{
		struct _MMEXTEND_INFO* ExtendInfo; // +0x18(0x4)
		void* BasedAddress; // +0x18(0x4)
	};
	EX_PUSH_LOCK SegmentLock; // +0x1c(0x4)
	ULONG u1; // +0x20(0x4)
	ULONG u2; // +0x24(0x4)
	struct _MMPTE* PrototypePte; // +0x28(0x4)
	//ULONGLONG ThePtes[0x1]; // +0x30(0x8)
};
struct _CONTROL_AREA // 0x50
{
	struct _SEGMENT* Segment; // +0x0(0x4)
	struct _LIST_ENTRY DereferenceList; // +0x4(0x8)
	ULONG NumberOfSectionReferences; // +0xc(0x4)
	ULONG NumberOfPfnReferences; // +0x10(0x4)
	ULONG NumberOfMappedViews; // +0x14(0x4)
	ULONG NumberOfUserReferences; // +0x18(0x4)
	ULONG  u; // +0x1c(0x4)
	ULONG FlushInProgressCount; // +0x20(0x4)
	struct _EX_FAST_REF FilePointer; // +0x24(0x4)  
};
struct _SUBSECTION // 0x20
{
	struct _CONTROL_AREA* ControlArea; // +0x0(0x4)
	struct _MMPTE* SubsectionBase; // +0x4(0x4)
	struct _SUBSECTION* NextSubsection; // +0x8(0x4)
	ULONG PtesInSubsection; // +0xc(0x4)
	ULONG UnusedPtes; // +0x10(0x4)
	struct _MM_AVL_TABLE* GlobalPerSessionHead; // +0x10(0x4)
	ULONG u; // +0x14(0x4)
	ULONG StartingSector; // +0x18(0x4)
	ULONG NumberOfFullSectors; // +0x1c(0x4)
};
typedef struct _MMVAD // 0x3c
{
	ULONG u1; // +0x0(0x4)
	struct _MMVAD* LeftChild; // +0x4(0x4)
	struct _MMVAD* RightChild; // +0x8(0x4)
	ULONG StartingVpn; // +0xc(0x4)
	ULONG EndingVpn; // +0x10(0x4)
	ULONG u; // +0x14(0x4)
	EX_PUSH_LOCK PushLock; // +0x18(0x4)
	ULONG u5; // +0x1c(0x4)
	ULONG u2; // +0x20(0x4)
	struct _SUBSECTION* Subsection; // +0x24(0x4)
	struct _MSUBSECTION* MappedSubsection; // +0x24(0x4)
	struct _MMPTE* FirstPrototypePte; // +0x28(0x4)
	struct _MMPTE* LastContiguousPte; // +0x2c(0x4)
	struct _LIST_ENTRY ViewLinks; // +0x30(0x8)
	struct _EPROCESS* VadsProcess; // +0x38(0x4)
}MMVAD;

typedef struct   tag_MM_AVL_TABLE // 0x20
{
	struct _MMADDRESS_NODE BalancedRoot; // +0x0(0x14)
	ULONG DepthOfTree; // +0x14(0x4)
	ULONG Unused; // +0x14(0x4)
	ULONG NumberGenericTableElements; // +0x14(0x4)
	void* NodeHint; // +0x18(0x4)
	void* NodeFreeHint; // +0x1c(0x4)
}MM_AVL_TABLE, *PMMAVL_TABLE;


#define GetVadRoot(eprocess)  ((PVOID)((char*)eprocess+0x278))

VOID PrintTree(MMVAD* Root)
{
	POBJECT_NAME_INFORMATION  Str = (POBJECT_NAME_INFORMATION)ExAllocatePool(PagedPool, 800);
	ULONG RetLen = 0;
	if (!Str || !Root)
		return;
	RtlZeroMemory(Str, 800);//递归要节省堆栈资源，不要大量使用局部变量
	__try
	{
		if (MmIsAddressValid(Root->Subsection) && MmIsAddressValid(Root->Subsection->ControlArea))
		{
			if (MmIsAddressValid((PVOID)Root->Subsection->ControlArea->FilePointer.Value))
			{
				PFILE_OBJECT pFileObj = (PFILE_OBJECT)((Root->Subsection->ControlArea->FilePointer.Value >> 3) << 3);
				if (MmIsAddressValid(pFileObj))
				{
					NTSTATUS Status = ObQueryNameString(pFileObj, Str, 800, &RetLen);
					if (NT_SUCCESS(Status))
					{
						KdPrint(("root:%X, obj:%X, Base:%08X Size:%dKb ", Root, pFileObj, Root->Subsection->ControlArea->Segment->BasedAddress, \
							(Root->Subsection->ControlArea->Segment->SizeOfSegment) / 0x1000));
						KdPrint(("Name:%ws\n", Str->Name.Buffer));
					}
					else
					{
						KdPrint(("不能获取到对象！%08X\n", Status));
					}

				}
			}

		}
	}
	__except (1)
	{
		KdPrint(("Invalid Address！\n"));
	}
	ExFreePool(Str);
	__try
	{
		if (MmIsAddressValid(Root->LeftChild))
			PrintTree(Root->LeftChild);
		if (MmIsAddressValid(Root->RightChild))
			PrintTree(Root->RightChild);
	}
	__except (1)
	{
		KdPrint(("异常！！"));
		return;
	}

}
VOID EnumProcessModule(ULONG Pid)
{
	DbgBreakPoint();
	PMMAVL_TABLE Table;
	PEPROCESS Epr = 0;
	if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)Pid, &Epr)))
	{
		KeAttachProcess(Epr);
		Table = (PMMAVL_TABLE)GetVadRoot(Epr);
		if (Table->BalancedRoot.LeftChild)
			PrintTree((MMVAD*)Table->BalancedRoot.LeftChild);
		if (Table->BalancedRoot.RightChild)
			PrintTree((MMVAD*)Table->BalancedRoot.RightChild);
		KeDetachProcess();
	}
}

VOID DriverUnLoad(PDRIVER_OBJECT Driver)
{
	KdPrint(("Driver UnLoaded!\n"));
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING Reg)
{
	KdPrint(("Enter Driver Entry!\n"));
	pDriverObject->DriverUnload = DriverUnLoad;
	EnumProcessModule(2880);

	return STATUS_SUCCESS;
}