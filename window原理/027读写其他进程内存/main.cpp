// 内存管理_操作其它进程的内存.cpp : 定义控制台应用程序的入口点。
//


#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD	dwPid = 0;
	HANDLE	hCalc = NULL;
	LPVOID	pRemoteBuff = NULL;

	// 根据窗口句柄获取进程pid
	GetWindowThreadProcessId(FindWindow(NULL, _T("计算器")), &dwPid);
	if (dwPid == 0) {
		STARTUPINFO si = { sizeof(STARTUPINFO) };
		PROCESS_INFORMATION pi = { 0 };
		CreateProcess(_T("win32calc.exe"), 0, 0, 0, 0, 0, 0, 0, &si, &pi);
		hCalc = pi.hProcess;
	}
	else {
		hCalc = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE,
			FALSE,
			dwPid);
		if (hCalc == NULL)
			return 0;
	}

	// 在其它进程中开辟内存空间
	pRemoteBuff = VirtualAllocEx(hCalc,  /* 要开辟内存空间的进程句柄,句柄必须有PROCESS_VM_OPERATION权限 */
		NULL,   /* 在指定地址处开辟内存空间,传NULL表示由系统自己选择 */
		4096,   /* 开辟的小 */
		MEM_RESERVE | MEM_COMMIT, /* 开辟空间的标志 */
		PAGE_EXECUTE_READWRITE     /* 内存分页的保护属性 */
		);

	if (pRemoteBuff == NULL) {
		printf("在远程进程开辟内存失败\n");
		return 0;
	}


	// 将数据写入内存
	DWORD dwWrite = 0;
	WriteProcessMemory(hCalc,
		pRemoteBuff,
		"Hello 15PB",
		20,
		&dwWrite);

	printf("可以使用windbg附加计算器,使用以下命令查看内存地址: da 0x%08X\n", pRemoteBuff);
	system("pause");

	// 释放内存
	VirtualFreeEx(hCalc, pRemoteBuff, 0, MEM_RELEASE);
	// 关闭句柄
	CloseHandle(hCalc);

	return 0;
}

