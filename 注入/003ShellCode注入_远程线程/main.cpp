#include <stdio.h>
#include <Windows.h>
#include <tchar.h>

bool InjectShellCode(DWORD dwPid, const BYTE* pShellCode, DWORD dwSize);

int _tmain(int argc, _TCHAR* argv[])
{
	BYTE shellcode[] =
	{
		"\xE8\x00\x00\x00\x00"	// call $+5
		"\x58"					// pop	eax
		"\x6A\x00"				// push	0
		"\x83\xC0\x17"			// add eax,0x17
		"\x50"					// push eax
		"\x83\xC0\x14"			// add eax,0x14
		"\x50"					// push eax
		"\x6A\x00"				// push 0
		"\xB8\x30\x88\x20\x74"	// mov eax,MessageBoxA
		"\xFF\xD0"				// call eax
		"\xC2\x04\x00"			// ret 0x04
		"来自shellcode的问候\0"  //20个字节
		"大家好,我是shellcode!!!" //24个字节
	};

	// 将MessageBoxA函数的地址写入到shellcode中.
	*(DWORD*)(shellcode + 19) = (DWORD)&MessageBoxA;


	DWORD	dwPid;
	printf("输入要注入到的进程PID:");
	scanf_s("%d[*]", &dwPid);

	// 注入设计好的shellcode
	InjectShellCode(dwPid, shellcode, sizeof(shellcode));

	return 0;
}

bool InjectShellCode(DWORD dwPid, const BYTE* pShellCode, DWORD dwSize)
{

	// shellcode注入的方式和DLL注入的方式差不多.
	// 但是shellcode注入的要求比较高, 需要自己编写shellcode
	// 注入步骤:
	// 1. 在远程进程中开辟内存空间
	// 2. 将shellcode写入到远程进程的内存空间中
	// 3. 创建远程线程,将被写入到远程进程内存shellcode的首地址
	//    作为线程回调函数的地址.
	// 4. 等待线程退出
	// 5. 销毁远程进程内存.

	bool	bRet = false;
	HANDLE	hProcess = 0;
	HANDLE	hRemoteThread = 0;
	LPVOID	pRemoteBuff = NULL;
	DWORD	dwWrite = 0;



	// 打开进程
	hProcess = OpenProcess(
		PROCESS_ALL_ACCESS,
		FALSE,
		dwPid /*进程ID*/
		);

	if (hProcess == NULL) {
		printf("打开进程失败,可能由于本程序的权限太低,请以管理员身份运行再尝试\n");
		goto _EXIT;
	}


	// 1. 在远程进程上开辟内存空间
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

	// 2. 将DLL路径写入到新开的内存空间中
	WriteProcessMemory(
		hProcess,
		pRemoteBuff,			  /* 要写入的地址 */
		pShellCode,		      /* 要写入的内容的地址 */
		dwSize,				  /* 写入的字节数 */
		&dwWrite				  /* 输出:函数实际写入的字节数 */
		);
	if (dwWrite != dwSize) {
		printf("写入DLL路径失败\n");
		goto _EXIT;
	}

	//3. 创建远程线程,
	//   远程线程创建成功后,DLL就会被加载,DLL被加载后DllMain函数
	//	 就会被执行,如果想要执行什么代码,就在DllMain中调用即可.
	hRemoteThread = CreateRemoteThread(
		hProcess,
		0, 0,
		(LPTHREAD_START_ROUTINE)pRemoteBuff,  /* 线程回调函数 */
		0,							           /* 回调函数参数 */
		0, 0);

	// 等待远程线程退出.
	// 退出了才释放远程进程的内存空间.
	WaitForSingleObject(hRemoteThread, -1);


	bRet = true;


_EXIT:
	// 释放远程进程的内存
	VirtualFreeEx(hProcess, pRemoteBuff, 0, MEM_RELEASE);
	// 关闭进程句柄
	CloseHandle(hProcess);

	return bRet;


}

