#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <tchar.h>

int g_nNum = 0;
HANDLE g_hMutex = NULL;


unsigned int __stdcall ThreadProc(void *pArg)
{
	int nIndex = (int)pArg;

	WaitForSingleObject(g_hMutex, INFINITE);

	++g_nNum;                                          
	Sleep(1); 
	printf("[%d] g_nNum = %d\n", nIndex, g_nNum);

	ReleaseMutex(g_hMutex);

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