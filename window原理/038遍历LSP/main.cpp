#include <winsock2.h>
#include <windows.h>
#include <tchar.h>
#include <locale.h>
#include <stdio.h>

// 多字符转换为宽字符(Unicode  --> ASCII)
#define  WCHAR_TO_CHAR(lpW_Char, lpChar) \
	WideCharToMultiByte(CP_ACP, NULL, lpW_Char, -1, lpChar, _countof(lpChar), NULL, FALSE);

// 多字符转换为宽字符(ASCII --> Unicode)
#define  CHAR_TO_WCHAR(lpChar, lpW_Char) \
	MultiByteToWideChar(CP_ACP, NULL, lpChar, -1, lpW_Char, _countof(lpW_Char));

typedef struct tagPACKED_CATALOG_ITEM
{
	char spi_path[MAX_PATH];
	WSAPROTOCOL_INFO protocol_info;
} PACKED_CATALOG_ITEM, *PPACKED_CATALOG_ITEM;

//获取分层服务在什么目录下
void GetCurrentProtocolCatalogPath(TCHAR *pszCatelogPath, int nccSize)
{
	DWORD type = REG_SZ;
	TCHAR buffer[MAX_PATH] = { 0 };
	DWORD chData = sizeof(buffer);

	//
	HKEY hValueKey = NULL;
	RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		_T("System\\CurrentControlSet\\Services\\Winsock2\\Parameters"),
		0,
		KEY_QUERY_VALUE,
		&hValueKey);
	if (NULL == hValueKey) return;


	RegQueryValueEx(
		hValueKey,
		_T("Current_Protocol_Catalog"),
		0,
		&type,
		(LPBYTE)buffer,
		&chData);
	_stprintf_s(pszCatelogPath, nccSize, _T("%s\\%s\\Catalog_Entries"), _T("System\\CurrentControlSet\\Services\\Winsock2\\Parameters"), buffer);
	//
	RegCloseKey(hValueKey);
}


//枚举LSP
void EnumLsp(TCHAR *szEntryPath)
{
	HKEY hResult = NULL;  //用来接收打开键的句柄
	RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		szEntryPath,
		0,
		KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE,
		&hResult
		);
	if (NULL == hResult) return;


	DWORD dwIndex = 0;
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

		//获取具体的Lsp项
		TCHAR szMidReg[MAX_PATH] = { 0 };
		_stprintf_s(szMidReg, _countof(szMidReg), _T("%s%s%s"), szEntryPath, _T("\\"), szNewKeyName);

		//打开具体项
		DWORD dwRegType;
		HKEY hValueKey = NULL;
		RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			szMidReg,
			0,
			KEY_QUERY_VALUE,
			&hValueKey);

		if (NULL != hValueKey)
		{
			TCHAR szRegValue[MAX_PATH];
			ZeroMemory(szRegValue, sizeof(szRegValue));
			DWORD dwRealLen = _countof(szRegValue);

			//获取服务名
			RegQueryValueEx(
				hValueKey,
				_T("ProtocolName"),
				0,
				&dwRegType,
				(LPBYTE)szRegValue,
				&dwRealLen);
			_tprintf(_T("服务名字:%s\n"), szRegValue);

			//获取dll的全路径名
			PACKED_CATALOG_ITEM catelogItem;
			ZeroMemory(&catelogItem, sizeof(catelogItem));
			dwRealLen = sizeof(catelogItem);
			RegQueryValueEx(
				hValueKey,
				_T("PackedCatalogItem"),
				0,
				&dwRegType,
				(LPBYTE)&catelogItem,
				&dwRealLen);
			//
			TCHAR szWinDllPath[MAX_PATH] = { 0 };
#ifdef _UNICODE
			CHAR_TO_WCHAR(catelogItem.spi_path, szWinDllPath);
#else
			strcpy_s(szWinDllPath, _countof(szWinDllPath), catelogItem.spi_path);
#endif
			TCHAR szAbDllPath[MAX_PATH] = { 0 };
			ExpandEnvironmentStrings(szWinDllPath, szAbDllPath, _countof(szAbDllPath));
			_tprintf(_T("dll路径:%s\n"), szAbDllPath);
			RegCloseKey(hValueKey);
		}
		dwIndex++;
	}
	if (NULL != hResult) 	RegCloseKey(hResult);
}



int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "chs"); //支持中文本地化
	TCHAR szCateLogPath[MAX_PATH] = { 0 };
	GetCurrentProtocolCatalogPath(szCateLogPath, _countof(szCateLogPath));
	EnumLsp(szCateLogPath);
	return 0;
}