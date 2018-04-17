#include <windows.h>
#include <tchar.h>
#include   <stdarg.h>  
#include <Shlwapi.h>
#include <Commctrl.h>
#include "resource.h"
#pragma  comment(lib, "Shlwapi.lib")

void PrintDbgInfo(LPCTSTR lptstrFormat, ...);

INT_PTR CALLBACK DialogProc(
	_In_  HWND hwndDlg,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	);



//////////////////////////////////////////////
HINSTANCE g_hIntance = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	g_hIntance = hInstance;
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN_DLG), NULL, DialogProc);
	return 0;
}

INT_PTR CALLBACK DialogProc(
	_In_  HWND hwndDlg,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	)
{

	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
	}
		break;
	case WM_HSCROLL:
	{
					   WORD wHigh = HIWORD(wParam);// pos
					   WORD wLow = LOWORD(wParam);// 状态
					   if (SB_THUMBTRACK == wLow)
					   {
						   SendDlgItemMessage(hwndDlg, IDC_PROGRESS1, PBM_SETPOS, wHigh, 0);
					   }
	}
		break;
	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
		break;
	default:
		break;
	}
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 多字符转换为宽字符(Unicode  --> ASCII)
#define  WCHAR_TO_CHAR(lpW_Char, lpChar) \
	WideCharToMultiByte(CP_ACP, NULL, lpW_Char, -1, lpChar, _countof(lpChar), NULL, FALSE);

// 多字符转换为宽字符(ASCII --> Unicode)
#define  CHAR_TO_WCHAR(lpChar, lpW_Char) \
	MultiByteToWideChar(CP_ACP, NULL, lpChar, -1, lpW_Char, _countof(lpW_Char));

//格式化输出信息字符串到输出缓冲区
void SprintfDbgInfo(TCHAR *pOutBuff, int nMaxOutBuffSize, LPCTSTR lptstrFormat, va_list argList)
{
	//
	const int nFileBuffSize = 256;
	TCHAR fileNameBuff[nFileBuffSize];

	//设置文件名和行号
#ifdef UNICODE
	CHAR_TO_WCHAR(__FILE__, fileNameBuff);
	TCHAR *pFileName = PathFindFileName(fileNameBuff);
	_stprintf_s(pOutBuff, nMaxOutBuffSize - sizeof(TCHAR), _T("[mod:%s:line:%d]  ::  "), pFileName, __LINE__);
#else
	_tcscpy_s(fileNameBuff, __FILE__);
	TCHAR *pFileName = PathFindFileName(fileNameBuff);
	_stprintf_s(pOutBuff, nMaxOutBuffSize - sizeof(TCHAR), _T("[mod:%s:line:%d]  ::"), pFileName, __LINE__);
#endif

	//设置输入信息
	int nHeadLen = _tcslen(pOutBuff);
	_vstprintf_s(pOutBuff + nHeadLen, nMaxOutBuffSize - sizeof(TCHAR)-nHeadLen * sizeof(TCHAR), lptstrFormat, argList);

	//设置换行
	int nBodyLen = _tcslen(pOutBuff);
	_stprintf_s(pOutBuff + nBodyLen, nMaxOutBuffSize - sizeof(TCHAR)-nBodyLen * sizeof(TCHAR), _T("\r\n"));
}

//打印出调试信息
void PrintDbgInfo(LPCTSTR lptstrFormat, ...)
{
	const int c_nMaxBuffSize = 4096;
	TCHAR outBuff[c_nMaxBuffSize];

	//把结果输出到缓冲区
	va_list argList;
	va_start(argList, lptstrFormat);
	SprintfDbgInfo(outBuff, c_nMaxBuffSize, lptstrFormat, argList);
	va_end(argList);

	//输出信息
	OutputDebugString(outBuff);
}
