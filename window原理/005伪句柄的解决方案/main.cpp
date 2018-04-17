#include <windows.h>
#include <process.h>
#include <stdio.h>


unsigned int __stdcall  ThreadProc(void * Parameter)
{
	while (true)
	{

		Sleep(1000);
	}
	return 0;
}

int main(int argc, char* argv[])
{

	HANDLE hThread = (HANDLE)_beginthreadex(NULL, NULL, ThreadProc, NULL, NULL, NULL);

	printf("获取本进程的句柄，伪句柄:%x\n", GetCurrentProcess());
	printf("获取本线程的句柄，伪句柄:%x\n", GetCurrentThread());

	HANDLE hTargetProcess = NULL;  //由伪句柄转化成的真实句柄
	DuplicateHandle(
		GetCurrentProcess(), //源进程句柄 
		GetCurrentProcess(), //要被处理的伪句柄
		GetCurrentProcess(), //目标进程句柄
		&hTargetProcess,
		NULL,
		FALSE,
		DUPLICATE_SAME_ACCESS);

	HANDLE hTargetThread = NULL;  //由伪句柄转化成的真实句柄
	DuplicateHandle(
		GetCurrentProcess(), //源进程句柄 
		GetCurrentThread(), //要被处理的伪句柄
		GetCurrentProcess(), //目标进程句柄
		&hTargetThread,
		NULL,
		FALSE,
		DUPLICATE_SAME_ACCESS);

	printf("获取本进程的句柄，真实句柄:%x\n", hTargetProcess);
	CloseHandle(hTargetProcess);
	printf("获取本线程的句柄，真实句柄:%x\n", hTargetThread);
	CloseHandle(hTargetThread);


	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);
	return 0;
}