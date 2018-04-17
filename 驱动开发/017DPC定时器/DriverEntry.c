#pragma warning(disable:4054)

#include <ntifs.h>
VOID DpcTimberCallBackProc(KDPC *Dpc, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2);
#pragma  alloc_text(NONE_PAGE, DpcTimberCallBackProc)

#define  MAX_DPCTIMER_COUNT 1000
#define MAX_PROCESSOR_COUNT 32

//获取每个核的kPrcb
typedef KAFFINITY (*KESETAFFINITYTHREAD)(
	__inout PKTHREAD Thread,
	__in KAFFINITY Affinity
);
ULONG g_KiProcessorBlock[MAX_PROCESSOR_COUNT];
BOOLEAN PsGetKiProcessorBlock()
{
	ULONG Index, Affinity, CurrentAffinity;
	ULONG fnpKeSetAffinityThread;
	ULONG kPrcb;
	UNICODE_STRING usFuncName;
	RtlInitUnicodeString(&usFuncName, L"KeSetAffinityThread");
	fnpKeSetAffinityThread = (ULONG)MmGetSystemRoutineAddress(&usFuncName);

	if (fnpKeSetAffinityThread == 0)
	{
		return FALSE;
	}

	Affinity = KeQueryActiveProcessors(); 

	CurrentAffinity = 1;
	Index = 0;
	while (Affinity)
	{
		//下面只是个简单的算法，使当前线程运行到不同的处理器上
		Affinity &= ~CurrentAffinity;
		((KESETAFFINITYTHREAD)fnpKeSetAffinityThread)(PsGetCurrentThread(), (KAFFINITY)CurrentAffinity);
		CurrentAffinity <<= 1;

		__asm
		{
			push eax;
			mov eax, fs:[0x20];
			mov kPrcb, eax
			pop eax
		}
		//得到我们要的东西
		g_KiProcessorBlock[Index] = kPrcb;
		Index++;
	}
	return TRUE;
}

typedef struct _KDPCTIMER
{
	DISPATCHER_HEADER  Header;
	LARGE_INTEGER DueTime;
	LIST_ENTRY TimerListEntry;
	PKDPC Dpc;
	ULONG Period;
}KDPCTIMER, *PKDPCTIMER;


typedef struct _KTIMER_TABLE_ENTRY_WIN7
{
	ULONG Lock;
	LIST_ENTRY Entry;
	LARGE_INTEGER Time;
}KTIMER_TABLE_ENTRY_WIN7, *PKTIMER_TABLE_ENTRY_WIN7;


ULONG EnumDpcTimer()
{
	ULONG                       TimerTableOffsetInKprcb;
	ULONG                       NumberOfTimerTable;
	ULONG                       NumberOfProcessor;
	ULONG                       i, j;
	ULONG                       ulTemp;
	PULONG                      pKiProcessorBlock = NULL;
	PKTIMER_TABLE_ENTRY_WIN7    pTimerTableEntryWin7 = NULL;
	PLIST_ENTRY                 pNextList = NULL;
	PKDPCTIMER                  pTimer = NULL;


	/*
	KPRCB->
	+0x1950 PeriodicBias     : 0
	+0x1958 TickOffset       : 0xce1f
	+0x1960 TimerTable       : _KTIMER_TABLE

	nt!_KTIMER_TABLE
	+0x000 TimerExpiry      : [16] Ptr32 _KTIMER
	+0x040 TimerEntries     : [256] _KTIMER_TABLE_ENTRY

	nt!_KTIMER_TABLE_ENTRY
	+0x000 Lock             : Uint4B
	+0x004 Entry            : _LIST_ENTRY
	+0x010 Time             : _ULARGE_INTEGER

	nt!_KTIMER
	+0x000 Header           : _DISPATCHER_HEADER
	+0x010 DueTime          : _ULARGE_INTEGER
	+0x018 TimerListEntry   : _LIST_ENTRY
	+0x020 Dpc              : Ptr32 _KDPC
	+0x024 Period           : Uint4B


	*/

	TimerTableOffsetInKprcb = 0x1960 + 0x40;                                                      //首个_KTIMER_TABLE_ENTRY在PRCB中的偏移
	NumberOfTimerTable = 0x100;                                                                 //_KTIMER_TABLE_ENTRY数量

	NumberOfProcessor = (ULONG)KeNumberProcessors;                                              //当前机器处理器数量
	if (NumberOfProcessor > MAX_PROCESSOR_COUNT) return 0;


	pKiProcessorBlock = (PULONG)&g_KiProcessorBlock[0];                                          //取得KiProcessorBlock,包含了NumberOfProcessor个KPRCB
	if (pKiProcessorBlock == NULL) return 0;


	for (i = 0; i < NumberOfProcessor; i++, pKiProcessorBlock++)                               //DPC timer在每个cpu中都有一个队列,所以枚举每一个KPRCB
	{
		if (!MmIsAddressValid((PVOID)pKiProcessorBlock)) goto __EXIT;        //检测一下当前的KPRCB地址是否可访问


		ulTemp = *pKiProcessorBlock + TimerTableOffsetInKprcb;                                  //取得当前CPU的KPRCB中首个KTIMER_TABLE_ENTRY地址
		if (!MmIsAddressValid((PVOID)ulTemp)) goto __EXIT;


		pTimerTableEntryWin7 = (PKTIMER_TABLE_ENTRY_WIN7)ulTemp;                                //此时ulTemp是 timer table entry地址

		for (j = 0; j < NumberOfTimerTable; j++, pTimerTableEntryWin7++)                       //准备遍历timer table表
		{
			if (!MmIsAddressValid((PVOID)pTimerTableEntryWin7)) goto __EXIT;
			if (pTimerTableEntryWin7->Time.HighPart == 0xFFFFFFFF) continue;                   //为空的数组高位双字为FFFFFFFF


			if (!MmIsAddressValid((PVOID)pTimerTableEntryWin7->Entry.Blink)) continue;
			if (!MmIsAddressValid((PVOID)pTimerTableEntryWin7->Entry.Flink)) continue;


			for (pNextList = (PLIST_ENTRY)pTimerTableEntryWin7->Entry.Blink;
				pNextList != (PLIST_ENTRY)&pTimerTableEntryWin7->Entry;
				pNextList = pNextList->Blink)
			{
				pTimer = CONTAINING_RECORD(pNextList, KDPCTIMER, TimerListEntry);                 //取得timer对象

				if (!MmIsAddressValid((PVOID)pTimer) ||
					!MmIsAddressValid((PVOID)pTimer->Dpc) ||
					!MmIsAddressValid((PVOID)pTimer->Dpc->DeferredRoutine))
				{
					if (!MmIsAddressValid(pNextList->Blink)) break;
					continue;
				}

				KdPrint(("dpc func :%X\n", pTimer->Dpc->DeferredRoutine));

				if (!MmIsAddressValid((PVOID)pNextList->Blink)) break;

			}// end while
		}// end for
	}// end for
__EXIT:
	return 0;
}


typedef struct DpcTImberExtend
{
	PDEVICE_OBJECT pDeviceObj;
	UNICODE_STRING strDeviceName;
	UNICODE_STRING strDeSymbolLinkName;
	KTIMER kTimber;
	KDPC kDpc;
}DPCTIMBER_EXTEND;


DPCTIMBER_EXTEND g_dpcExtend = { 0 };
//卸载函数
VOID DriverUnload(PDRIVER_OBJECT pDirver)
{
	UNREFERENCED_PARAMETER(pDirver);
	KeCancelTimer(&g_dpcExtend.kTimber);
}


static ULONG nCount = 0;
VOID DpcTimberCallBackProc(KDPC *Dpc, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArgument2)
{

	UNREFERENCED_PARAMETER(Dpc);
	UNREFERENCED_PARAMETER(DeferredContext);
	UNREFERENCED_PARAMETER(SystemArgument1);
	UNREFERENCED_PARAMETER(SystemArgument2);

	DbgPrint("这是调用的第%d次\n", nCount);
	nCount++;
	LARGE_INTEGER timeOut;
	timeOut.QuadPart = -30000000;
	KeSetTimer(&g_dpcExtend.kTimber, timeOut, &g_dpcExtend.kDpc);

}


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING path){


	DbgBreakPoint();
	PsGetKiProcessorBlock();
	UNREFERENCED_PARAMETER(path);
	NTSTATUS nStatuss = STATUS_UNSUCCESSFUL;
	UNICODE_STRING temName = RTL_CONSTANT_STRING(L"\\Device\\TestDpc");
	g_dpcExtend.strDeviceName = temName;
	pDriver->DriverUnload = DriverUnload;
	nStatuss = IoCreateDevice(pDriver, 0, &g_dpcExtend.strDeviceName, FILE_DEVICE_UNKNOWN, 0, 0, &g_dpcExtend.pDeviceObj);

	if (!NT_SUCCESS(nStatuss))
	{
		DbgPrint("创建设备失败\n");
	}
	//初始化DpcTimber
	KeInitializeTimer(&g_dpcExtend.kTimber);

	//初始化dpc对象

	KeInitializeDpc(&g_dpcExtend.kDpc, DpcTimberCallBackProc, NULL);

	LARGE_INTEGER timeOut;
	timeOut.QuadPart = -30000000;
	KeSetTimer(&g_dpcExtend.kTimber, timeOut, &g_dpcExtend.kDpc);

	EnumDpcTimer();


	return STATUS_SUCCESS;

}