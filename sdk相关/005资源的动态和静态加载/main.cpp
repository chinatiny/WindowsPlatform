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
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
						  //1. 图标
						  HICON  hIcon = LoadIcon(g_hIntance, MAKEINTRESOURCE(IDI_ICON1));
						  SetClassLong(hwndDlg, GCL_HICON, (LONG)hIcon);

						  //2. 光标
						  HCURSOR hCusor = LoadCursor(g_hIntance, MAKEINTRESOURCE(IDC_CURSOR1));
						  SetClassLong(hwndDlg, GCL_HCURSOR, (LONG)hCusor);

						  //3. 添加菜单，通过设置Dialog的menu属性来设置

						  //4. 动态添加菜单，在按钮的事件中处理了


						  //5. 弹出菜单, 在WM_RBUTTONDOWN消息中处理了

						  //6.加速键 -> 这个需要在消息循环中处理  == 011控件消息截获中有例子程序
						  HACCEL hAccel = LoadAccelerators(g_hIntance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
						  //如下
						  /*
						       HACCEL hAccel = LoadAccelerators(g_hIntance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
								while (GetMessage (&msg, NULL, 0, 0))

								{

								if (!TranslateAccelerator (hwnd, hAccel, &msg))

								{

								TranslateMessage (&msg) ;

								DispatchMessage (&msg) ;

								}

								}


						  '*/
	
						  
						  //7.字符串表
						  TCHAR szStringBuff1[MAX_PATH];
						  LoadString(g_hIntance, IDS_STRING111, szStringBuff1, _countof(szStringBuff1) - 1);
						  PrintDbgInfo(_T("string table:%s"), szStringBuff1);

						  TCHAR szStringBuff[MAX_PATH];
						  LoadString(g_hIntance, IDS_STRING01, szStringBuff, _countof(szStringBuff) - 1);
						  PrintDbgInfo(_T("string table:%s"), szStringBuff);

	}
		break;
	case WM_COMMAND:
	{
					   WORD wNotifyCode = HIWORD(wParam);
					   WORD wCtlID = LOWORD(wParam);
					   switch (wCtlID)
					   {
					   case IDC_DYN_LOAD_BUTTON:   //动态加载菜单
					   {
													   HMENU hDynMenu = LoadMenu(g_hIntance, MAKEINTRESOURCE(IDR_DYN_MENU));
													   SetMenu(hwndDlg, hDynMenu);
					   }
						   break;
					   case IDR_ACCELERATOR1:
					   {
												MessageBox(hwndDlg, _T("加速键被执行"), _T("提示"), MB_OK);
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
	case WM_RBUTTONDOWN:
	{
						   POINT pt;
						   GetCursorPos(&pt);

						   HMENU hTop = LoadMenu(g_hIntance, MAKEINTRESOURCE(IDR_POP_MENU));
						   HMENU hSubMenu = GetSubMenu(hTop, 0);
						   TrackPopupMenu(hSubMenu, TPM_LEFTALIGN, pt.x, pt.y, 0, hwndDlg, NULL);
	}
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
