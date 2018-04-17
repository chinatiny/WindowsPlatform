#include <windows.h>
#include <tchar.h>
#include <stdio.h>


int main(int argc, char* argv[])
{
	HANDLE hEvent = NULL;
	hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, _T("010事件_防止多开_可以被SetEvent破解"));
	//
	if (NULL != hEvent)
	{
		printf("程序已经有一个实例了\n");
		goto __EXIT_END;
	}
	hEvent = CreateEvent(NULL, FALSE, TRUE, _T("010事件_防止多开_可以被SetEvent破解"));
	//
	printf("程序执行中\n");
	while (true);

__EXIT_END:
	system("pause");
	return 0;
}