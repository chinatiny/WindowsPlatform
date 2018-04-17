#include <windows.h>
#include <tchar.h>
#include <stdio.h>


int main(int argc, char* argv[])
{
	HANDLE hMutex = NULL;
	hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("008互斥体"));

	if (NULL == hMutex)
	{
		hMutex = CreateMutex(NULL, TRUE, _T("008互斥体"));
		printf("源进程id:%d 源句柄id:%d\n", GetCurrentProcessId(), hMutex);
		//
		printf("程序运行中\n");
		while (true);
	}
	else
	{
		printf("另外一个程序已经在运行,关闭程序\n");
	}
	CloseHandle(hMutex);

	system("pause");
	return 0;
}