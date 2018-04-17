#include <stdio.h>
#include <windows.h>

bool DllInject(DWORD dwPid)
{
	/*
	远程线程注入：
	在目标进程中开启一个远程线程：CreateRemoteThread
	线程回掉函数正好是一个参数的函数
	LoadLibrary正好也是一个参数的函数
	所以就能够让远线程调用LoadLibrary
	*/
	DWORD dwRealWrite = 0;
	//1 打开一个进程
	HANDLE hProcess =
		OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);

	//2 在目标进程中申请一块空间
	LPVOID pBuf =
		VirtualAllocEx(hProcess, 0, 1, MEM_COMMIT, PAGE_READWRITE);

	//3 把Dll的路径写入到目标进程申请出的空间中
	WriteProcessMemory(
		hProcess,
		pBuf,
		L"c:/001测试注入的动态库.dll",
		(wcslen(L"c:/001测试注入的动态库.dll") + 1) * 2,
		&dwRealWrite
		);

	//4 创建一个远程线程，在目标程序中创建LoadLibraryW.
	__asm int 3
	HANDLE hThread = CreateRemoteThread(
		hProcess,
		0, 0,
		(LPTHREAD_START_ROUTINE)LoadLibraryW,
		pBuf,
		0, 0
		);
	//5 等待线程结束
	WaitForSingleObject(hThread, -1);
	//6 收尾工作
	VirtualFreeEx(hProcess, pBuf, 1, 0);
	CloseHandle(hThread);
	CloseHandle(hProcess);
	return true;
}

int main()
{
	int nId = 0;
	printf("请输入你要注入的进程ID:");
	scanf_s("%d", &nId);
	DllInject(nId);
	return 0;
}

