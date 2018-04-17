#pragma  once
#include <windows.h>

#define  WCHAR_TO_CHAR(lpW_Char, lpChar) \
	WideCharToMultiByte(CP_ACP, NULL, lpW_Char, -1, lpChar, _countof(lpChar), NULL, FALSE);

// ¶à×Ö·û×ª»»Îª¿í×Ö·û(ASCII --> Unicode)
#define  CHAR_TO_WCHAR(lpChar, lpW_Char) \
	MultiByteToWideChar(CP_ACP, NULL, lpChar, -1, lpW_Char, _countof(lpW_Char));

#pragma  comment(lib, "user32.lib")
#pragma  comment(lib, "Advapi32.lib")
