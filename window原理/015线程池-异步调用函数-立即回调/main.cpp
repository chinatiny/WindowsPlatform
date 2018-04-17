#include <windows.h>
#include <tchar.h>
#include <stdio.h>

typedef NTSTATUS(WINAPI *fpNtQueryInfomationThread)(
	HANDLE ThreadHandle,
	ULONG ThreadInformationClass,
	PVOID ThreadInformation,
	ULONG ThreadInformationLength,
	PULONG ReturnLength);
typedef enum _THREADINFOCLASS {
	ThreadBasicInformation,
	ThreadTimes,
	ThreadPriority,
	ThreadBasePriority,
	ThreadAffinityMask,
	ThreadImpersonationToken,
	ThreadDescriptorTableEntry,
	ThreadEnableAlignmentFaultFixup,
	ThreadEventPair_Reusable,
	ThreadQuerySetWin32StartAddress,
	ThreadZeroTlsCell,
	ThreadPerformanceCount,
	ThreadAmILastThread,
	ThreadIdealProcessor,
	ThreadPriorityBoost,
	ThreadSetTlsArrayAddress,   // Obsolete  
	ThreadIsIoPending,
	ThreadHideFromDebugger,
	ThreadBreakOnTermination,
	ThreadSwitchLegacyState,
	ThreadIsTerminated,
	ThreadLastSystemCall,
	ThreadIoPriority,
	ThreadCycleTime,
	ThreadPagePriority,
	ThreadActualBasePriority,
	ThreadTebInformation,
	ThreadCSwitchMon,          // Obsolete  
	ThreadCSwitchPmu,
	ThreadWow64Context,
	ThreadGroupInformation,
	ThreadUmsInformation,      // UMS  
	ThreadCounterProfiling,
	ThreadIdealProcessorEx,
	MaxThreadInfoClass
} THREADINFOCLASS;

static fpNtQueryInfomationThread s_fpNtQueryInfomationThread = NULL;
HANDLE s_hEvent = NULL;
LPVOID s_lpThreadAddress = NULL;




VOID  NTAPI Proc1(PTP_CALLBACK_INSTANCE Instance, PVOID Context)
{

	printf("[%d] %d\n",
		(int)Context, GetThreadId(GetCurrentThread()));

	DWORD dwCurrentThreadID = GetCurrentThreadId();
	HANDLE hThread = NULL;
	hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, dwCurrentThreadID);
	LPVOID lpThreadAddress = NULL;
	DWORD dwReturnLength = 0;
	s_fpNtQueryInfomationThread(hThread,
		ThreadQuerySetWin32StartAddress,
		&lpThreadAddress,
		sizeof(lpThreadAddress),
		&dwReturnLength);
	s_lpThreadAddress = lpThreadAddress;
	SetEvent(s_hEvent);
}


int main(int argc, char* argv[])
{
	s_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	HMODULE hNtdll = GetModuleHandle(_T("ntdll.dll"));
	s_fpNtQueryInfomationThread = (fpNtQueryInfomationThread)GetProcAddress(hNtdll, "NtQueryInformationThread");
	//第一种方式:
	for (int i = 0; i < 1; ++i) 
	{
		// 将提交回调函数到线程池
		TrySubmitThreadpoolCallback( Proc1 , (PVOID)i , NULL );
	}
	WaitForSingleObject(s_hEvent, INFINITE);
	
	system("pause");
	return 0;
}