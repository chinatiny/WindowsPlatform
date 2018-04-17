#include <windows.h>
#include <tchar.h>
#include   <stdarg.h>  
#include <Shlwapi.h>
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

	//1. 静态图片加载需要修改 pic ctrl的type属性为对应的图片类型， 
	//2. bitmap只支持24色
	//3. center image 只有设置为true时候才可以拖动图片

	//4. 	vs2013在把picture control 设置为bitmap的时候会有bug，需要找到如下的代码，添加:"Static", SS_BITMAP,并且每次修改资源都要修改一次，否则窗体无法启动
	//CONTROL         IDB_BITMAP1, IDC_STATIC1, "Static", SS_BITMAP, 21, 20, 349, 138, SS_CENTERIMAGE | NOT WS_GROUP
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
	}
		break;
	case WM_COMMAND:
	{
					   WORD wNotifyCode = HIWORD(wParam);
					   WORD wCtlID = LOWORD(wParam);
					   switch (wCtlID)
					   {
					   case IDC_BUTTON1:
					   {
										   static bool bFlag = true;
										   //
										   HBITMAP hBtm = LoadBitmap(g_hIntance, MAKEINTRESOURCE(bFlag?IDB_BITMAP2:IDB_BITMAP1));
										   SendDlgItemMessage(hwndDlg, IDC_STATIC1, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBtm);
										   DeleteObject(hBtm);

										   bFlag = !bFlag;

					   }
						   break;
					   default:
						   break;
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
