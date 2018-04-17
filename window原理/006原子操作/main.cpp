#include <windows.h>
#include <stdio.h>
#include <process.h>

int n = 0;

unsigned int __stdcall  ThreadProc1(void * Parameter)
{
	for (int i = 0; i < 1000000; i++)
	{
		InterlockedIncrement((long*)&n);
	}
	return 0;
}

unsigned int __stdcall  ThreadProc2(void * Parameter)
{
	for (int i = 0; i < 1000000; i++)
	{
		//InterlockedIncrement((long*)&n);
		//  =>> 可以被替换为
		__asm
		{
			lock inc n
		}
	}
	return 0;
}



int main(int argc, char* argv[])
{
	HANDLE hThread[2] = {0};

	hThread[0] = (HANDLE)_beginthreadex(NULL, NULL, ThreadProc1, NULL, NULL, NULL);
	hThread[1] = (HANDLE)_beginthreadex(NULL, NULL, ThreadProc2, NULL, NULL, NULL);
	WaitForMultipleObjects(_countof(hThread), hThread, TRUE, INFINITE);
	printf("n:%d\n", n);
	system("pause");

	return 0;
}