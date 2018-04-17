#include <windows.h>
#include <stdio.h>

VOID NTAPI TimerProc(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context,
	_Inout_     PTP_TIMER             Timer
	)
{
	printf("[%d] %d %d\n", Timer, Context, GetCurrentThreadId());
}


int main(int argc, char* argv[])
{
	PTP_TIMER ptrTimer = CreateThreadpoolTimer(
		TimerProc,
		NULL,
		NULL);

	FILETIME time = {0};
	SYSTEMTIME sysTime;
	// 	sysTime.wYear = 2019;
	// 	sysTime.wMonth = 8;
	// 	sysTime.wDay = 1;
	// 	sysTime.wHour = 12;
	// 	sysTime.wMinute = 12;
	// 	sysTime.wSecond = 12;
	// 	SystemTimeToFileTime( &sysTime , &time );
	time.dwLowDateTime = -1;
	SetThreadpoolTimer(
		ptrTimer,
		&time,
		1000, //º‰∏Ù ±º‰
		10    //ŒÛ≤Ó
		);
	printf("curent thread id:%d\n", GetCurrentThreadId());

	system("pause");
	CloseThreadpoolTimer(ptrTimer);

	return 0;
}