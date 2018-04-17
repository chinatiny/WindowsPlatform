#include <ntifs.h>


VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	KdPrint(("Leave"));
}


// 根据PID返回进程EPROCESS，失败返回NULL
PEPROCESS LookupProcess(HANDLE hPid)
{
	PEPROCESS pEProcess = NULL;
	if (NT_SUCCESS(PsLookupProcessByProcessId(
		hPid, &pEProcess)))
		return pEProcess;
	return NULL;
}

//暂停进程
NTKERNELAPI NTSTATUS PsSuspendProcess(
	PEPROCESS pEProcess);

VOID MySyspendProcess(ULONG uPid)
{
	PEPROCESS pProcess = LookupProcess((HANDLE)uPid);
	if (pProcess != NULL)
	{
		PsSuspendProcess(pProcess);
	}
	ObDereferenceObject(pProcess);
}

//恢复进程

NTKERNELAPI NTSTATUS PsResumeProcess(
	PEPROCESS pEProcess);
VOID MyResumeProcess(ULONG uPid)
{
	PEPROCESS pProcess = LookupProcess((HANDLE)uPid);
	if (pProcess != NULL)
	{
		PsResumeProcess(pProcess);
	}
	ObDereferenceObject(pProcess);
}

void KernelKillProcess(ULONG uPid) {
	HANDLE            hProcess = NULL;
	CLIENT_ID         ClientId = { 0 };
	OBJECT_ATTRIBUTES objAttribut =
	{ sizeof(OBJECT_ATTRIBUTES) };
	ClientId.UniqueProcess = (HANDLE)uPid; // PID
	ClientId.UniqueThread = 0;
	// 打开进程，如果句柄有效，则结束进程
	ZwOpenProcess(
		&hProcess,    // 返回打开后的句柄
		1,            // 访问权限
		&objAttribut, // 对象属性
		&ClientId);   // 进程ID结构
	if (hProcess) {
		ZwTerminateProcess(hProcess, 0);
		ZwClose(hProcess);
	};
}
NTKERNELAPI HANDLE PsGetProcessInheritedFromUniqueProcessId(
	IN PEPROCESS pEProcess);
NTKERNELAPI UCHAR* PsGetProcessImageFileName(
	IN PEPROCESS pEProcess);
VOID EnumProcess() {
	PEPROCESS pEProc = NULL;
	// 循环遍历进程（假设线程的最大值不超过0x25600）
	ULONG i = 0;
	for (i = 4; i < 0x25600; i = i + 4) {
		// a.根据PID返回PEPROCESS
		pEProc = LookupProcess((HANDLE)i);
		if (!pEProc) continue;
		// b. 打印进程信息
		DbgPrint("EPROCESS=%p PID=%ld PPID=%ld Name=%s\n",
			pEProc, (UINT32)PsGetProcessId(pEProc),
			(UINT32)PsGetProcessInheritedFromUniqueProcessId(pEProc),
			PsGetProcessImageFileName(pEProc));
		// c. 将进程对象引用计数减1
		ObDereferenceObject(pEProc);
		DbgPrint("\n");
	}
}


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	KdPrint(("Hello 15PB\n"));

	DbgBreakPoint();
	//MyResumeProcess(1836);
	//KernelKillProcess(1836);
	EnumProcess();

	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}