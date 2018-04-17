// 020禁用掉DLL_THREAD_ATTACH.cpp : 定义控制台应用程序的入口点。
//

/*
	typedef NTSTATUS (NTAPI *_ZwCreateThreadEx)(
	OUT PHANDLE ThreadHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN HANDLE ProcessHandle,
	IN PTHREAD_START_ROUTINE StartRoutine,
	IN PVOID StartContext,
	IN ULONG CreateThreadFlags,
	IN SIZE_T ZeroBits OPTIONAL,
	IN SIZE_T StackSize OPTIONAL,
	IN SIZE_T MaximumStackSize OPTIONAL,
	IN PPROC_THREAD_ATTRIBUTE_LIST AttributeList
	);

	这里不关心其他参数的使用，只关心绿色的flags参数.
	当这个参数不是0的时候，我的系统上线程总是会创建后立刻暂停，所以使用该参数为其他值的时候，需要使用ResumeThread让线程跑起来。

	因为类型是ULONG,所以参数CreateThreadFlags除了让线程停止之外应该具有其他功能。
	用IDA把目光瞄准内核代码，发现这个参数会影响ETHREAD中的一些东西，
	恰巧的是传入值为2的时候，可以绕过DLL_ATTACH_THREAD,大家都懂得。
	另外就是传入值为4的时候，貌似OD里对这个线程里的代码下断会飞，似乎是hidefromdbgger的功能。
	8的时候，没测试出东西来，不过flags最大就是0xF,再大就创建线程失败了。
*/


#include "stdafx.h"
#include <windows.h>

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR Buffer;
}UNICODE_STRING, *PUNICODE_STRING;

typedef struct _PEB_LDR_DATA
{
	ULONG Length;
	BOOLEAN Initialized;
	PVOID SsHandle;
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _PEB { // Size: 0x1D8
	UCHAR InheritedAddressSpace;
	UCHAR ReadImageFileExecOptions;
	UCHAR BeingDebugged;
	UCHAR SpareBool; // Allocation size
	HANDLE Mutant;
	HINSTANCE ImageBaseAddress; // Instance
	PEB_LDR_DATA *DllList;
	PVOID *ProcessParameters;
	ULONG SubSystemData;
	HANDLE DefaultHeap;
	KSPIN_LOCK FastPebLock;
	ULONG FastPebLockRoutine;
	ULONG FastPebUnlockRoutine;
	ULONG EnvironmentUpdateCount;
	ULONG KernelCallbackTable;
	LARGE_INTEGER SystemReserved;
	ULONG FreeList;
	ULONG TlsExpansionCounter;
	ULONG TlsBitmap;
	LARGE_INTEGER TlsBitmapBits;
	ULONG ReadOnlySharedMemoryBase;
	ULONG ReadOnlySharedMemoryHeap;
	ULONG ReadOnlyStaticServerData;
	ULONG AnsiCodePageData;
	ULONG OemCodePageData;
	ULONG UnicodeCaseTableData;
	ULONG NumberOfProcessors;
	LARGE_INTEGER NtGlobalFlag; // Address of a local copy
	LARGE_INTEGER CriticalSectionTimeout;
	ULONG HeapSegmentReserve;
	ULONG HeapSegmentCommit;
	ULONG HeapDeCommitTotalFreeThreshold;
	ULONG HeapDeCommitFreeBlockThreshold;
	ULONG NumberOfHeaps;
	ULONG MaximumNumberOfHeaps;
	ULONG ProcessHeaps;
	ULONG GdiSharedHandleTable;
	ULONG ProcessStarterHelper;
	ULONG GdiDCAttributeList;
	KSPIN_LOCK LoaderLock;
	ULONG OSMajorVersion;
	ULONG OSMinorVersion;
	USHORT OSBuildNumber;
	USHORT OSCSDVersion;
	ULONG OSPlatformId;
	ULONG ImageSubsystem;
	ULONG ImageSubsystemMajorVersion;
	ULONG ImageSubsystemMinorVersion;
	ULONG ImageProcessAffinityMask;
	ULONG GdiHandleBuffer[0x22];
	ULONG PostProcessInitRoutine;
	ULONG TlsExpansionBitmap;
	UCHAR TlsExpansionBitmapBits[0x80];
	ULONG SessionId;
} PEB, *PPEB;

typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union
	{
		LIST_ENTRY HashLinks;
		PVOID SectionPointer;
	};
	ULONG CheckSum;
	union
	{
		ULONG TimeDateStamp;
		PVOID LoadedImports;
	};
	PVOID EntryPointActivationContext;
	PVOID PatchInformation;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

BOOL MyDisableThreadLibraryCalls(HMODULE hMod)
{
	PEB* pPEB = NULL;
	PEB_LDR_DATA* pDllList = NULL;
	LIST_ENTRY* pFlink = NULL;
	LIST_ENTRY* pIndex = NULL;
	LDR_DATA_TABLE_ENTRY* pEntry = NULL;

	__asm
	{
		mov eax, fs:[0x30]
			mov pPEB, eax
	}
	pDllList = pPEB->DllList;

	//InLoadOrderLinks
	pFlink = &pDllList->InLoadOrderLinks;
	pIndex = pFlink->Flink;
	pEntry = NULL;
	while (pIndex != pFlink)
	{
		pEntry = CONTAINING_RECORD(pIndex, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
		if (hMod == pEntry->DllBase)
		{
			pEntry->Flags |= 0x40000;
			pEntry->Flags |= 0x80000;
		}
		pIndex = pEntry->InLoadOrderLinks.Flink;
	}

	//InMemoryOrderModuleList
	pFlink = &pDllList->InMemoryOrderModuleList;
	pIndex = pFlink->Flink;
	pEntry = NULL;
	while (pIndex != pFlink)
	{
		pEntry = CONTAINING_RECORD(pIndex, LDR_DATA_TABLE_ENTRY, InMemoryOrderModuleList);
		if (hMod == pEntry->DllBase)
		{
			pEntry->Flags |= 0x40000;
			pEntry->Flags |= 0x80000;
		}
		pIndex = pEntry->InMemoryOrderModuleList.Flink;
	}

	//InInitializationOrderModuleList
	pFlink = &pDllList->InInitializationOrderModuleList;
	pIndex = pFlink->Flink;
	pEntry = NULL;
	while (pIndex != pFlink)
	{
		pEntry = CONTAINING_RECORD(pIndex, LDR_DATA_TABLE_ENTRY, InInitializationOrderModuleList);
		if (hMod == pEntry->DllBase)
		{
			pEntry->Flags |= 0x40000;
			pEntry->Flags |= 0x80000;
		}
		pIndex = pEntry->InInitializationOrderModuleList.Flink;
	}

	return TRUE;
}


int _tmain(int argc, _TCHAR* argv[])
{
	MyDisableThreadLibraryCalls((HMODULE)GetModuleHandle(NULL));
	return 0;
}

