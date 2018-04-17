#include <windows.h>
#include <tchar.h>
#include <locale.h>
#include <Imm.h>

#pragma  comment(lib, "Imm32.lib")


//遍历启动项
void EnumInputMethod(HKEY hRootKey, LPCTSTR szSubKey)
{
	HKEY hResult = NULL;  //用来接收打开键的句柄

	LONG lReturn = RegOpenKeyEx(
		hRootKey,
		szSubKey,
		0,
		KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE,
		&hResult
		);
	if (ERROR_SUCCESS != lReturn) return;
	DWORD dwIndex = 0;
	while (true)
	{
		TCHAR szInputKey[MAX_PATH] = { 0 };
		DWORD InputValueNo = _countof(szInputKey);
		TCHAR szInstallNo[MAX_PATH] = { 0 };
		DWORD dwcbData = sizeof(szInstallNo);
		DWORD dwType = 0;
		LONG lReturn = RegEnumValue(
			hResult,
			dwIndex,
			szInputKey,
			&InputValueNo,
			NULL,
			&dwType,
			(PBYTE)szInstallNo,
			&dwcbData
			);
		if (ERROR_SUCCESS != lReturn) break;

		//获取输入法具体信息
		TCHAR *szPartKeyPath = _T("SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts\\");
		TCHAR szFullRegKey[MAX_PATH] = { 0 };
		_stprintf_s(szFullRegKey, _countof(szFullRegKey), _T("%s%s"), szPartKeyPath, szInstallNo);
		_tprintf(_T("输入法的NO:%s\n"), szInstallNo);

		HKEY hValueKey = NULL;
		RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			szFullRegKey,
			0,
			KEY_QUERY_VALUE,
			&hValueKey);
		if (NULL != hValueKey)
		{
			TCHAR szRegValue[MAX_PATH] = { 0 };
			ZeroMemory(szRegValue, sizeof(szRegValue));
			DWORD dwRealLen = _countof(szRegValue);
			DWORD dwRegType;

			ZeroMemory(szRegValue, sizeof(szRegValue));
			//获取输入法的Ime文件
			RegQueryValueEx(
				hValueKey,
				_T("Ime File"),
				0,
				&dwRegType,
				(LPBYTE)szRegValue,
				&dwRealLen);
			if (_tcscmp(szRegValue, _T("")) == 0)
			{
				RegQueryValueEx(
					hValueKey,
					_T("Layout Display Name"),
					0,
					&dwRegType,
					(LPBYTE)szRegValue,
					&dwRealLen);
				_tprintf(_T("输入法的IME文件:%s\n"), szRegValue);
			}
			else
			{
				_tprintf(_T("输入法的IME文件:%s\n"), szRegValue);
			}
			//获取输入法名字
			RegQueryValueEx(
				hValueKey,
				_T("Layout Text"),
				0,
				&dwRegType,
				(LPBYTE)szRegValue,
				&dwRealLen);
			_tprintf(_T("输入法的名字:%s\n"), szRegValue);

			ZeroMemory(szRegValue, sizeof(szRegValue));
			//获取输入法的Layout文件
			RegQueryValueEx(
				hValueKey,
				_T("Layout File"),
				0,
				&dwRegType,
				(LPBYTE)szRegValue,
				&dwRealLen);
			_tprintf(_T("输入法的布局文件:%s\n"), szRegValue);

			RegCloseKey(hValueKey);
		}
		dwIndex++;
	}
	if (NULL != hResult) RegCloseKey(hResult);
}


//获取当前输入法
void GetCurrentImme()
{
	HKL hkl;
	TCHAR szInputName[MAX_PATH] = { 0 };
	hkl = GetKeyboardLayout(GetCurrentThreadId());
	if (ImmIsIME(hkl))
	{
		//需要在窗口程序才有效
		ImmGetDescription(hkl, szInputName, _countof(szInputName));
		_tprintf(_T("当前激活的输入法:%s\n"), szInputName);
	}
}


int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "chs"); //支持中文本地化
	system("pause");
	GetCurrentImme();
	EnumInputMethod(HKEY_CURRENT_USER, _T("Keyboard Layout\\Preload"));
	system("pause");
	return 0;
}