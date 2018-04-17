#include <windows.h>
#include <tchar.h>
#include <process.h>
#include <stdio.h>


int main(int argc, char* argv[])
{
	HANDLE hMutex = NULL;
	hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("008互斥体"));

	while (true)
	{
		DWORD dwTargetProcessID;
		DWORD dwMutexHandle;
		printf("请输入客户端的进程id:");
		scanf_s("%d", &dwTargetProcessID);
		fflush(stdin);
		printf("请输入互斥体句柄id:");
		scanf_s("%d", &dwMutexHandle);
		HANDLE hTargetProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwTargetProcessID);
		if (hTargetProcess == INVALID_HANDLE_VALUE)
		{
			printf("打开进程失败, err code:%d\n", GetLastError());
		}
		//
		HANDLE hSelfMutex = NULL;
		DuplicateHandle(
			hTargetProcess,
			(HANDLE)dwMutexHandle,
			GetCurrentProcess(),
			&hSelfMutex,
			0,
			FALSE,
			DUPLICATE_CLOSE_SOURCE);

		CloseHandle(hSelfMutex);

	}
	return 0;
}