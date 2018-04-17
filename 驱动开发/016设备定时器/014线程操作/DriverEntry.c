#include <ntifs.h>


VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	KdPrint(("Leave"));
}

KEVENT g_kEvent;
VOID t_funThread(IN PVOID StartContext) {
	// 1. 打印测试字符
	PUNICODE_STRING pustrMsg = (PUNICODE_STRING)StartContext;
	DbgPrint("Kernel thread: %d\n", PsGetCurrentThreadId());
	DbgPrint("Kernel thread: %wZ\n", pustrMsg);
	// 2. 将事件对象置为授信状态，并结束本线程
	KeSetEvent(&g_kEvent, 0, TRUE);
	PsTerminateSystemThread(STATUS_SUCCESS);
}
VOID Test_CreateThread() {
	NTSTATUS       Status;
	HANDLE         hThread;
	UNICODE_STRING ustrMsg = RTL_CONSTANT_STRING(L"15PB!");
	// 1. 初始化事件后创建线程
	DbgPrint("Kernel thread: %d\n", PsGetCurrentThreadId());
	KeInitializeEvent(&g_kEvent, SynchronizationEvent, FALSE);
	Status = PsCreateSystemThread(&hThread, 0, NULL, NULL, NULL,
		t_funThread, (PVOID)&ustrMsg);
	if (!NT_SUCCESS(Status))  return;
	ZwClose(hThread);
	// 2. 等待事件对象信号
	KeWaitForSingleObject(&g_kEvent, Executive, KernelMode, 0, 0);
}
//暂停线程


//恢复线程


//结束线程


NTSTATUS ZwOpenThread(
	_Out_  PHANDLE ThreadHandle,
	_In_   ACCESS_MASK DesiredAccess,
	_In_   POBJECT_ATTRIBUTES ObjectAttributes,
	_In_   PCLIENT_ID ClientId);
typedef NTSTATUS(__fastcall *ZWTERMINATETHREAD)(
	HANDLE hThread,
	ULONG uExitCode);
ZWTERMINATETHREAD ZwTerminateThread = (ZWTERMINATETHREAD)0x8407c6cb; //函数地址,需要自己去寻找
void KernelKillThread() {
	HANDLE            hThread = NULL;
	CLIENT_ID         ClientId = { 0 };
	OBJECT_ATTRIBUTES objAttribut =
	{ sizeof(OBJECT_ATTRIBUTES) };
	ClientId.UniqueProcess = 0;
	ClientId.UniqueThread = (HANDLE)1234; // TID  
	//打开线程，如果句柄有效，则结束线程
	ZwOpenThread(&hThread, 1, &objAttribut, &ClientId);
	if (hThread) {
		ZwTerminateThread(hThread, 0);
		ZwClose(hThread);
	}
}

// 根据TID返回线程ETHREAD，失败返回NULL
PETHREAD LookupThread(HANDLE hTid)
{
	PETHREAD pEThread = NULL;
	if (NT_SUCCESS(PsLookupThreadByThreadId(
		hTid,
		&pEThread)))
		return pEThread;
	return NULL;
}

VOID EnumThread(PEPROCESS pEProcess) {
	PEPROCESS pEProc = NULL;
	PETHREAD  pEThrd = NULL;
	// 循环遍历线程（假设线程的最大值不超过0x25600）
	ULONG i = 0;
	for (i = 4; i < 0x25600; i += 4) {
		// a. 根据TID返回ETHREAD
		pEThrd = LookupThread((HANDLE)i);
		if (!pEThrd)  continue;
		// b. 获得线程所属进程，如果相等则打印线程信息
		pEProc = IoThreadToProcess(pEThrd);
		if (pEProc == pEProcess) {
			DbgPrint("[THREAD]ETHREAD=%p TID=%ld\n",
				pEThrd, (ULONG)PsGetThreadId(pEThrd));
		}
		// c. 将线程对象引用计数减1
		ObDereferenceObject(pEThrd);
	}
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


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	KdPrint(("Hello 15PB\n"));

	DbgBreakPoint();
	PEPROCESS pProcess = LookupProcess((HANDLE)3536);
	EnumThread(pProcess);


	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}
