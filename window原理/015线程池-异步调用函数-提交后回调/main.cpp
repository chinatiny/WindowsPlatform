#include <windows.h>
#include <stdio.h>

VOID NTAPI proc2(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context,
	_Inout_     PTP_WORK              Work
	)
{
	for (int i = 0; i < 100000; i++)
		printf("[%d] %d\n", (int)Context, GetThreadId(GetCurrentThread()));
}


int main(int argc, char* argv[])
{
	PTP_WORK pThreadPool = CreateThreadpoolWork(proc2, NULL, 0);

	for (int i = 0; i < 100; i++)
	{
		SubmitThreadpoolWork(pThreadPool);
	}

	system("pause");
	CloseThreadpoolWork(pThreadPool);

	return 0;
}