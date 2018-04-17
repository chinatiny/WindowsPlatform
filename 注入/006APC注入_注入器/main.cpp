#include <windows.h>
#include <stdio.h>


bool InjectShellCode(DWORD dwPid, const char* szDllName);

int main(int argc, char* argv[])
{
	DWORD	dwPid;
	printf("输入要注入到的进程PID:");
	scanf_s("%d[*]", &dwPid);
	fflush(stdin);
	InjectShellCode(dwPid, "F:\\15pb练习\\Windows平台\\Debug\\001测试注入的动态库.dll");
	return 0;
}

bool InjectShellCode(DWORD dwPid, const char* szDllName)
{
	bool	bRet = false;
	HANDLE	hInjectProcess = NULL;
	HANDLE hInjectThread = NULL;
	LPVOID	pRemoteBuff = NULL;
	DWORD	dwWrite = 0;

	// 1. 打开进程
	hInjectProcess = OpenProcess(
		PROCESS_ALL_ACCESS,
		FALSE,
		dwPid /*进程ID*/
		);
	if (hInjectProcess == NULL) 
	{
		printf("打开进程失败,可能由于本程序的权限太低,请以管理员身份运行再尝试\n");
		goto _EXIT;
	}

	// 2. 在远程进程上开辟内存空间
	pRemoteBuff = VirtualAllocEx(
		hInjectProcess,
		NULL,
		64 * 1024,
		MEM_COMMIT,
		PAGE_EXECUTE_READWRITE
		);
	if (pRemoteBuff == NULL)
	{
		printf("在远程进程上开辟空降失败\n");
		goto _EXIT;
	}

	// 3. 将DLL路径写入到新开的内存空间中
	WriteProcessMemory(
		hInjectProcess,
		pRemoteBuff,
		szDllName,
		strlen(szDllName) + 1,
		&dwWrite);
	if (ERROR_SUCCESS != GetLastError()) 
	{
		printf("写入DLL路径失败\n");
		goto _EXIT;
	}

	DWORD	dwInjectThreadID = 0;
	printf("输入要注入到的可警醒的线程ID:");
	scanf_s("%d[*]", &dwInjectThreadID);
	fflush(stdin);

	//4. 打开线程
	hInjectThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwInjectThreadID);
	if (NULL == hInjectThread)
	{
		printf("打开线程失败\n");
		goto _EXIT;
	}

	//5. 向目标线程投递异步apc任务
	QueueUserAPC((PAPCFUNC)LoadLibraryA, hInjectThread, (DWORD)pRemoteBuff);

	bRet = true;

_EXIT:
	// 这里没法释放没法预料对方什么时候执行完毕
	//VirtualFreeEx(hInjectProcess, pRemoteBuff, 0, MEM_RELEASE);
	// 关闭进程句柄
	CloseHandle(hInjectProcess);
	//关闭线程句柄
	CloseHandle(hInjectThread);

	return bRet;
}
