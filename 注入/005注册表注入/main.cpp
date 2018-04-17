#include <windows.h>
#include <tchar.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	HKEY hKey = NULL;
	LONG lResult = 0;

	//1. 打开注册表项
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
	if (ERROR_SUCCESS != lResult)
	{
		printf("打开注册表失败");
		goto __EXIT;
	}


	//2. 设置AppInit_DLLs的键值为我们的Dll
	TCHAR szDllPath[] = _T("F:\\15pb练习\\Windows平台\\Debug\\001测试注入的动态库.dll");
	lResult = RegSetValueEx(hKey,
		_T("AppInit_DLLs"),
		0,
		REG_SZ,
		(BYTE*)szDllPath,
		sizeof(szDllPath)
		);
	if (lResult != ERROR_SUCCESS)
	{
		printf("设置注册表失败");
		goto __EXIT;
	}


__EXIT:
	if (hKey != NULL)  RegCloseKey(hKey);


	return 0;
}