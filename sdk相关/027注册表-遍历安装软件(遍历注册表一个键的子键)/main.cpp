#include <Windows.h>
#include <tchar.h>
#include <locale.h>

//遍历安装的软件
void EnumInstallSoftWare()
{
	LPCTSTR szSubKey = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
	//LPCTSTR szSubKey = _T("SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
	HKEY hRootKey = HKEY_LOCAL_MACHINE;
	HKEY hResult = NULL;  //用来接收打开键的句柄

	//1. 打开一个已经存在的注册表键
	LONG lReturn = RegOpenKeyEx(
		hRootKey,
		szSubKey,
		0,
		KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE,
		&hResult
		);
	DWORD dwIndex = 0;
	//2. 循环遍历Uninstall目录下的子键
	while (true)
	{
		DWORD dwKeyLen = MAX_PATH;
		TCHAR szNewKeyName[MAX_PATH];

		LONG lReturn = RegEnumKeyEx(
			hResult,
			dwIndex,
			szNewKeyName,
			&dwKeyLen,
			0,
			NULL,
			NULL,
			NULL
			);
		if (ERROR_SUCCESS != lReturn) break;
		//2.1 通过得到子键的名字拼接成新的子键路径
		TCHAR szMidReg[MAX_PATH] = { 0 };
		_stprintf_s(szMidReg, _countof(szMidReg), _T("%s%s%s"), szSubKey, _T("\\"), szNewKeyName);
		//2.2 打开新的子键获取其句柄
		DWORD dwRegType;
		HKEY hValueKey = NULL;
		RegOpenKeyEx(
			hRootKey,
			szMidReg,
			0,
			KEY_QUERY_VALUE,
			&hValueKey);
		//2.3 获取其键值：名字、
		TCHAR szRegValue[MAX_PATH];
		ZeroMemory(szRegValue, sizeof(szRegValue));
		DWORD dwRealLen = _countof(szRegValue);
		//获取软件名字
		RegQueryValueEx(
			hValueKey,
			_T("DisplayName"),
			0,
			&dwRegType,
			(LPBYTE)szRegValue,
			&dwRealLen);
		_tprintf(_T("软件名字:%s\n"), szRegValue);
		//获取卸载路径
		dwRealLen = _countof(szRegValue);
		ZeroMemory(szRegValue, sizeof(szRegValue));
		RegQueryValueEx(
			hValueKey,
			_T("UninstallString"),
			0,
			&dwRegType,
			(LPBYTE)szRegValue,
			&dwRealLen);
		_tprintf(_T("\t卸载路径:%s\n"), szRegValue);
		if (NULL != hValueKey) RegCloseKey(hValueKey);

		dwIndex++;
	}
	if (NULL != hResult) 	RegCloseKey(hResult);
}


int main(int argc, char * argv[])
{
	setlocale(LC_ALL, "chs"); //支持中文本地化
	_tprintf(_T("软件信息:\n"));
	EnumInstallSoftWare();
	system("pause");
	return 0;
}