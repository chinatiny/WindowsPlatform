// 003遍历所有窗口.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <tchar.h>


int _tmain(int argc, _TCHAR* argv[])
{

	HWND hWnd = GetDesktopWindow();
	hWnd = GetWindow(hWnd, GW_CHILD);
	TCHAR szName[MAX_PATH];

	while (hWnd != NULL)
	{
		memset(szName, 0, sizeof(szName));
		GetWindowText(hWnd, szName, sizeof(szName)-sizeof(TCHAR));
		_tprintf(_T("%s\n"), szName);
		hWnd = GetNextWindow(hWnd, GW_HWNDNEXT);
	}
	return 0;
}

