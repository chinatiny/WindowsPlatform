#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <tchar.h>

#pragma  warning(disable: 4723)

HANDLE g_hMutex = NULL;


unsigned int __stdcall ThreadProc(void *pArg)
{
	int nIndex = (int)pArg;

	WaitForSingleObject(g_hMutex, INFINITE);
	printf("-----------------------------------------------------------\n");
	printf("线程:%d,抢到互斥体,现在倒数5秒异常退出\n", nIndex);
	int n = 100;

	__try
	{
		while (true)
		{
			for (int i = 0; i < 5; i++)
			{
				Sleep(1000);
				printf("%d秒......\n", i + 1);
			}
			n /= 0;

		}
	}
	__except (1)
	{
		printf("我挂了,看清楚了我没释放互斥体哦(*^__^*) 嘻嘻……\n");
	}

	return 0;
}



int main(int argc, char* argv[])
{
	g_hMutex = CreateMutex(NULL, FALSE, _T("008互斥体同步"));
	if (INVALID_HANDLE_VALUE == g_hMutex)
	{
		printf("程序异常退出\n");
		goto __EXCEPTION_END;
	}

	HANDLE phThreadArray[10];
	for (int i = 0; i < 10; ++i)
	{
		phThreadArray[i] = (HANDLE)_beginthreadex(0, 0, ThreadProc, (void*)i, 0, 0);
	}

	WaitForMultipleObjects(10, phThreadArray, TRUE, INFINITE);
	CloseHandle(g_hMutex);


__EXCEPTION_END:
	system("pause");

	return 0;
}