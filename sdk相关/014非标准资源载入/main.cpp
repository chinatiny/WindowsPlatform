#include <windows.h>
#include <tchar.h>
#include <Windowsx.h>
#include "resource.h"
#include "Dbg.h"
#include <CommCtrl.h>

#pragma comment(lib,"Comctl32.lib ")

INT_PTR CALLBACK DialogProc(
	_In_  HWND hwndDlg,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	);

HINSTANCE g_hInstance = NULL;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	g_hInstance = hInstance;
	HWND hDlg = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc, NULL);
	ShowWindow(hDlg, SW_SHOW);

	MSG msg = { 0 };
	while (GetMessage(&msg, 0, 0, 0))
	{

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
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
	case WM_COMMAND:
	{
					   WORD wNotifyCode = HIWORD(wParam);
					   WORD wCtrlID = LOWORD(wParam);
					   switch (wCtrlID)
					   {
					   case IDC_BUTTON1:
					   {
										   HRSRC hSrc = FindResource(g_hInstance, MAKEINTRESOURCE(IDR_EXE1), _T("EXE"));
										   HGLOBAL hGlobal = LoadResource(g_hInstance, hSrc);
										   //exe在内存中的位置
										   LPVOID lpBuf = LockResource(hGlobal);
										   DWORD dwSize = SizeofResource(g_hInstance, hSrc);

										   // 释放
										   HANDLE hFile = CreateFile(_T("c:\\123.exe"), 
											   GENERIC_ALL, 
											   FILE_SHARE_READ,
											   NULL, 
											   CREATE_ALWAYS, 
											   FILE_ATTRIBUTE_NORMAL,
											   NULL);
										   DWORD dwWrite = 0;// 实际写入的字节数
										   WriteFile(hFile, lpBuf, dwSize, &dwWrite, NULL);
										   CloseHandle(hFile);
										   WinExec("c:\\123.exe", SW_SHOW);
										   return 1;
					   }
						   break;
					   default:
						   break;
					   }
					   return 1;
	}
		break;
	case WM_CLOSE:
	{
					 PrintDbgInfo(_T("WM_CLOSE 消息被触发"));
					 PostQuitMessage(0);
					 return 1;
	}
		break;
	case WM_NOTIFY:// 复杂控件的通知消息
	{
	}
	default:
		break;
	}
	return 0;
}
