#include <windows.h>
#include <winternl.h>
#pragma comment(lib,"ntdll.lib")
bool NQIP_CheckParentProcess()
{
	struct PROCESS_BASIC_INFORMATION {
		ULONG ExitStatus; 		 // 进程返回码
		PPEB  PebBaseAddress; 		 // PEB地址
		ULONG AffinityMask; 		 // CPU亲和性掩码
		LONG  BasePriority; 		 // 基本优先级
		ULONG UniqueProcessId; 		 // 本进程PID
		ULONG InheritedFromUniqueProcessId; // 父进程PID
	}stcProcInfo;

	NtQueryInformationProcess(GetCurrentProcess(),
		ProcessBasicInformation,
		&stcProcInfo,
		sizeof(stcProcInfo),
		NULL);

	DWORD ExplorerPID = 0;
	DWORD CurrentPID = stcProcInfo.InheritedFromUniqueProcessId;
	GetWindowThreadProcessId(FindWindow(L"Progman", NULL), &ExplorerPID);

	return ExplorerPID == CurrentPID ? false : true;
}

int main(int argc, char* argv[])
{
	bool bIsDebug = NQIP_CheckParentProcess();
	if (bIsDebug)
	{
		MessageBoxA(NULL, "正在被调试", "警告", 0);
	}
	else
	{
		MessageBoxA(NULL, "现在很安全", "恭喜", 0);
	}
	return 0;
}
