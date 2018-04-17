#include "RegAnti.h"
#include <windows.h>



//////////////////////////////对外接口///////////////////////////////////////
void GetAppInitDll(TCHAR *szDllNameBuff, int nccSize)
{
	HKEY hKey = NULL;
	LONG lResult = 0;
	ZeroMemory(szDllNameBuff, sizeof(TCHAR)* nccSize);
	/*
	注意：32位和64位注册表key不一样
	32位："SOFTWARE\\Wow6432Node\\Microsoft\\Windows NT\\CurrentVersion\\Windows"
	64位："SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows"
	*/
	lResult = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\Wow6432Node\\Microsoft\\Windows NT\\CurrentVersion\\Windows"),
		0,
		KEY_ALL_ACCESS,
		&hKey
		);
	if (ERROR_SUCCESS != lResult) return;

	//获取路径名字
	DWORD dwRegType = 0;
	TCHAR szRegValue[MAX_PATH];
	ZeroMemory(szRegValue, sizeof(szRegValue));
	DWORD dwRealLen = _countof(szRegValue);
	//获取软件名字
	RegQueryValueEx(
		hKey,
		_T("AppInit_DLLs"),
		0,
		&dwRegType,
		(LPBYTE)szRegValue,
		&dwRealLen);

	_tcscpy_s(szDllNameBuff, nccSize, szRegValue);
}
