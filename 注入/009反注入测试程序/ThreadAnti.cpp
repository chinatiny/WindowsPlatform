#include "ThreadAnti.h"
#include <windows.h>
#include <tchar.h>
#include "Dbg.h"
#include "ProcessOperation.h"
#include "UnDocoumentApi.h"
#include <set>
#include "detours/detours.h"
#pragma comment(lib,"detours/lib.X86/detours.lib")

//////////////////////////////类型定义///////////////////////////////////////
typedef DWORD(WINAPI * fpNtCreateThreadEx)(
	PHANDLE                 ThreadHandle,
	ACCESS_MASK             DesiredAccess,
	LPVOID                  ObjectAttributes,
	HANDLE                  ProcessHandle,
	LPTHREAD_START_ROUTINE  lpStartAddress,
	LPVOID                  lpParameter,
	BOOL                    CreateSuspended,
	DWORD                   dwStackSize,
	DWORD                   Unknown1,
	DWORD                   Unknown2,
	LPVOID                  Unknown3
	);

typedef  VOID(__fastcall *fpBaseThreadInitThunk)(
	LPTHREAD_START_ROUTINE lpStartAddress,
	LPVOID lpParameter,
	LPVOID UnKnow
);

typedef HANDLE (WINAPI *fpCreateRemoteThreadEx)(
_In_ HANDLE hProcess,
_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
_In_ SIZE_T dwStackSize,
_In_ LPTHREAD_START_ROUTINE lpStartAddress,
_In_opt_ LPVOID lpParameter,
_In_ DWORD dwCreationFlags,
_In_opt_ LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
_Out_opt_ LPDWORD lpThreadId
);

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

//////////////////////////////内部函数///////////////////////////////////////
static fpBaseThreadInitThunk s_fpSrcBaseThreadInitThunk = NULL;
static fpNtCreateThreadEx s_fpNtCreateThread = NULL;
static fpCreateRemoteThreadEx s_fpCreateRemoteThreadEx = NULL;
static fpNtQueryInfomationThread s_fpNtQueryInfomationThread = NULL;
static bool s_bRemoteThread = false; //是否发现远程线程


static std::set<LPTHREAD_START_ROUTINE> s_setThreadProcAddress;  //线程函数开始地址
static LPVOID s_lpThreadWorkAddress = NULL;
static HANDLE s_hThreadPoolEvent = NULL;



//监控线程第一次开始运行
static void MonitorThreadRun()
{
	s_bRemoteThread = true;
}

//山寨的NtCreateThreadEx
static DWORD WINAPI MyNtCreateThreadEx(
	PHANDLE                 ThreadHandle,
	ACCESS_MASK             DesiredAccess,
	LPVOID                  ObjectAttributes,
	HANDLE                  ProcessHandle,
	LPTHREAD_START_ROUTINE  lpStartAddress,
	LPVOID                  lpParameter,
	BOOL                    CreateSuspended,
	DWORD                   dwStackSize,
	DWORD                   Unknown1,
	DWORD                   Unknown2,
	LPVOID                  Unknown3
	)
{
	s_setThreadProcAddress.insert(lpStartAddress);
	return s_fpNtCreateThread(ThreadHandle,
		DesiredAccess,
		ObjectAttributes,
		ProcessHandle,
		lpStartAddress,
		lpParameter,
		CreateSuspended,
		dwStackSize,
		Unknown1,
		Unknown2,
		Unknown3
		);
}

//山寨版本的CreateRemoteThreadEx
HANDLE WINAPI MyCreateRemoteThreadEx(
	_In_ HANDLE hProcess,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ SIZE_T dwStackSize,
	_In_ LPTHREAD_START_ROUTINE lpStartAddress,
	_In_opt_ LPVOID lpParameter,
	_In_ DWORD dwCreationFlags,
	_In_opt_ LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
	_Out_opt_ LPDWORD lpThreadId
	)
{
	s_setThreadProcAddress.insert(lpStartAddress);
	return s_fpCreateRemoteThreadEx(hProcess,
		lpThreadAttributes,
		dwStackSize,
		lpStartAddress,
		lpParameter,
		dwCreationFlags,
		lpAttributeList,
		lpThreadId);
}

//山寨的BaseThreadInitThunk
static void __fastcall MyBaseThreadInitThunk(
	LPTHREAD_START_ROUTINE lpUnknow,
	LPVOID lpStartAddress,
	LPVOID UnKnow
	)
{
	if (s_setThreadProcAddress.find((LPTHREAD_START_ROUTINE)lpStartAddress) == s_setThreadProcAddress.end() &&
		lpStartAddress != s_lpThreadWorkAddress //线程池单独处理
		)
	{
		MonitorThreadRun();
	}	
	s_fpSrcBaseThreadInitThunk(lpUnknow, lpStartAddress, UnKnow);

}

//枚举线程的函数
static bool EnumThreadFunc(IN  void *callBackParameter, IN THREADENTRY32* pThreadEntry)
{
	DWORD dwSelfPID = GetCurrentProcessId();
	if (pThreadEntry->th32OwnerProcessID == dwSelfPID)
	{
		int *pnCount = (int*)callBackParameter;
		++*pnCount;
	}
	return false;
}

//获取线程池开始地址
static VOID  NTAPI ThreadPoolProc(PTP_CALLBACK_INSTANCE Instance, PVOID Context)
{
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
	s_lpThreadWorkAddress = lpThreadAddress;
	SetEvent(s_hThreadPoolEvent);
}

//////////////////////////////对外接口///////////////////////////////////////

//反程序运行时远程线程注入
void AntiRemoteThread()
{
	HMODULE hKernel32 = GetModuleHandle(_T("kernel32.dll"));
	HMODULE hNtdll = GetModuleHandle(_T("ntdll.dll"));
	s_fpSrcBaseThreadInitThunk = (fpBaseThreadInitThunk)GetProcAddress(hKernel32, "BaseThreadInitThunk");
	s_fpNtCreateThread = (fpNtCreateThreadEx)GetProcAddress(hNtdll, "NtCreateThreadEx");
	s_fpCreateRemoteThreadEx = (fpCreateRemoteThreadEx)GetProcAddress(hKernel32, "CreateRemoteThreadEx");
	s_fpNtQueryInfomationThread = (fpNtQueryInfomationThread)GetProcAddress(hNtdll, "NtQueryInformationThread");

	//获取线程池开始地址
	s_hThreadPoolEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	TrySubmitThreadpoolCallback(ThreadPoolProc, NULL, NULL);
	WaitForSingleObject(s_hThreadPoolEvent, INFINITE);
	CloseHandle(s_hThreadPoolEvent);
	s_hThreadPoolEvent = NULL;
	//开始hook
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((PVOID*)&s_fpSrcBaseThreadInitThunk, MyBaseThreadInitThunk);
	DetourAttach((PVOID*)&s_fpNtCreateThread, MyNtCreateThreadEx);
	DetourAttach((PVOID*)&s_fpCreateRemoteThreadEx, MyCreateRemoteThreadEx);
	DetourTransactionCommit();
	//把本线程id插入到集合中
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
	s_setThreadProcAddress.insert((LPTHREAD_START_ROUTINE)lpThreadAddress);
	CloseHandle(hThread);
}

//反程序运行之前外挂开启了线程
void CheckBeforOEPThread()
{
	int nThreadCount = 0;
	EnumThread(EnumThreadFunc, &nThreadCount);

	if (nThreadCount > 1)
	{
		s_bRemoteThread = true;
	}
}

//是否发现远程线程注入
bool IsFondRemoteThread()
{
	return s_bRemoteThread;
}
