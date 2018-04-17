#include <windows.h>
#include <process.h>
#include <tchar.h>
#include <stdio.h>
#include <locale.h>

// 监控
unsigned int __stdcall MonitorProcess(void* pArg)
{
	DWORD dwNeedMonitorProcessID = (DWORD)pArg;
	//
	while (true)
	{
		TCHAR szModuleName[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, szModuleName, sizeof(szModuleName));
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		TCHAR szProcessID[MAX_PATH];
		_itot_s(GetCurrentProcessId(), szProcessID, _countof(szProcessID), 10);
		_stprintf_s(szModuleName, _countof(szModuleName), _T("%s %s"), szModuleName, szProcessID);

		HANDLE hMoniHandle = NULL;
		//如果第一次执行
		if (0 == dwNeedMonitorProcessID)
		{
			CreateProcess(
				NULL,
				szModuleName,
				NULL,
				NULL,
				FALSE,
				CREATE_NEW_CONSOLE,
				NULL,
				NULL,
				&si,
				&pi);

			hMoniHandle = pi.hProcess;
			WaitForSingleObject(hMoniHandle, INFINITE);
		}
		//其余
		else
		{
			hMoniHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwNeedMonitorProcessID);
			if (INVALID_HANDLE_VALUE == hMoniHandle) break;
			WaitForSingleObject(hMoniHandle, INFINITE);

			CreateProcess(
				NULL,
				szModuleName,
				NULL,
				NULL,
				FALSE,
				CREATE_NEW_CONSOLE,
				NULL,
				NULL,
				&si,
				&pi);
			//
			dwNeedMonitorProcessID = pi.dwProcessId;
		}
	}
	printf("打开进程句柄失败失败, errocode:%d\n", GetLastError());
	return 0;
}

int _tmain(int argc, TCHAR * argv[])
{
	setlocale(LC_ALL, "chs"); //支持中文本地化
	if (argc <= 1)
	{
		_beginthreadex(0, 0, MonitorProcess, 0, 0, 0);
	}
	else
	{
		int nNeedMoniProcessID = _tcstoul(argv[1], NULL, 10);
		_beginthreadex(0, 0, MonitorProcess, (void*)nNeedMoniProcessID, 0, 0);
	}
	printf("进程运行中");
	while (true);

	return 0;
}