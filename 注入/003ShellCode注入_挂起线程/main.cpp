#include <stdio.h>
#include <Windows.h>
#include <tchar.h>
#include "ProcessOperation.h"

DWORD g_dwThreadID = 0;
bool injectShellcode(DWORD dwPid,  BYTE* pShellCode, DWORD dwSize);
bool EnumThreadProc(IN  void *callBackParameter, IN THREADENTRY32* pThreadEntry);

int _tmain(int argc, _TCHAR* argv[])
{
	BYTE shellcode[] =
	{
		"\x68\x00\x00\x00\x00"	// push 返回值
		"\xE8\x00\x00\x00\x00"	// call $+5
		"\x58"					// pop	eax
		"\x83\xC0\x0d"			// add eax,0xD
		"\x50"					// push eax
		"\xB8\x30\x88\x20\x74"	// mov eax,LoadLibraryA
		"\xFF\xD0"				// call eax
		"\xC3"			// ret 0x04
		"F:\\15pb练习\\Windows平台\\Debug\\001测试注入的动态库.dll" 
	};


	DWORD	dwPid;
	printf("输入要注入到的进程PID:");
	scanf_s("%d[*]", &dwPid);

	// 注入设计好的shellcode
	injectShellcode(dwPid, shellcode, sizeof(shellcode));

	return 0;
}

bool injectShellcode(DWORD dwPid,  BYTE* pShellCode, DWORD dwSize)
{

	bool	bRet = false;
	HANDLE	hProcess = NULL;
	LPVOID	pRemoteBuff = NULL;
	DWORD	dwWrite = 0;
	HANDLE hThread = NULL;
	CONTEXT threadContext = { 0 };

	//1. 枚举找出进程中的线程
	EnumThread(EnumThreadProc, &dwPid);
	if (0 == g_dwThreadID)
	{
		printf("没有找到指定进程的线程\n");
		goto _EXIT;
	}

	//2. 打开线程
	hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, g_dwThreadID);
	if (NULL == hThread)
	{
		printf("打开线程失败\n");
		goto _EXIT;
	}

	//3.挂起线程
	SuspendThread(hThread);
	
	//4. 获取线程环境
	threadContext.ContextFlags = CONTEXT_FULL;
	GetThreadContext(hThread, &threadContext);

	// 5. 打开进程
	hProcess = OpenProcess(
		PROCESS_ALL_ACCESS,
		FALSE,
		dwPid /*进程ID*/
		);
	if (NULL == hProcess)
	{
		printf("打开进程失败,可能由于本程序的权限太低,请以管理员身份运行再尝试\n");
		goto _EXIT;
	}

	//6. 修正shellcode
	*(DWORD*)(pShellCode + 1) = threadContext.Eip;
	// 将MessageBoxA函数的地址写入到shellcode中.
	*(DWORD*)(pShellCode + 16) = (DWORD)&LoadLibraryA;


	// 6. 在远程进程上开辟内存空间
	pRemoteBuff = VirtualAllocEx(
		hProcess,
		NULL,
		64 * 1024, /*大小:64Kb*/
		MEM_COMMIT,/*预定并提交*/
		PAGE_EXECUTE_READWRITE/*可读可写可执行的属性*/
		);
	if (pRemoteBuff == NULL)
	{
		printf("在远程进程上开辟空降失败\n");
		goto _EXIT;
	}

	// 7. 将shellcode写入
	WriteProcessMemory(
		hProcess,
		pRemoteBuff,			  /* 要写入的地址 */
		pShellCode,		      /* 要写入的内容的地址 */
		dwSize,				  /* 写入的字节数 */
		&dwWrite				  /* 输出:函数实际写入的字节数 */
		);
	if (dwWrite != dwSize) 
	{
		printf("写入shellcode失败\n");
		goto _EXIT;
	}

	//8. 把eip设置为shellcode  
	threadContext.Eip = (DWORD)pRemoteBuff;
	SetThreadContext(hThread, &threadContext);

	//9. 恢复线程
	ResumeThread(hThread);

	bRet = true;


_EXIT:
	// 释放远程进程的内存
	//VirtualFreeEx(hProcess, pRemoteBuff, 0, MEM_RELEASE);
	// 关闭进程句柄
	CloseHandle(hProcess);
	//关闭线程句柄
	CloseHandle(hThread);

	return bRet;


}

bool EnumThreadProc(IN  void *callBackParameter, IN THREADENTRY32* pThreadEntry)
{
	DWORD dwOwnerProcessID = (DWORD)(*(DWORD*)callBackParameter);
	if (pThreadEntry->th32OwnerProcessID == dwOwnerProcessID)
	{
		g_dwThreadID = pThreadEntry->th32ThreadID;
		return true;
	}
	return false;
}

