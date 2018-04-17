#include <windows.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	unsigned int uCount = 0;
	DWORD dwThreadID = GetCurrentThreadId();
	DWORD dwProcessID = GetCurrentProcessId();
	while (true)
	{
		if (uCount > 0 && uCount % 1000 == 0)
		{
			printf("当前进程id:%d, 当前线程id为:%d\n", dwProcessID, dwThreadID);
			SleepEx(3000, TRUE);
		}
		++uCount;
	}
	return 0;
}