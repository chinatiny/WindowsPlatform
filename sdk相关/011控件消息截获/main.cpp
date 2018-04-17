#include <windows.h>
#include <tchar.h>
#include <Windowsx.h>
#include "resource.h"
#include "Dbg.h"

INT_PTR CALLBACK DialogProc(
	_In_  HWND hwndDlg,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	);

INT_PTR CALLBACK BtnProc(
	_In_ HWND   hwndDlg,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
	);
WNDPROC g_oldProc = nullptr;

HINSTANCE g_hInstance = NULL;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	g_hInstance = hInstance;
	HWND hDlg = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc, NULL);
	ShowWindow(hDlg, SW_SHOW);

	HACCEL hAccTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	MSG msg = { 0 };
	while (GetMessage(&msg, 0, 0, 0))
	{
		if (!TranslateAccelerator(hDlg, hAccTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
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

						  HWND hButton = GetDlgItem(hwndDlg, IDC_BUTTON1);
						  g_oldProc = (WNDPROC)SetWindowLong(hButton, GWL_WNDPROC, (LONG)BtnProc);
	}
		break;
	case WM_COMMAND:
	{
					   WORD wHigh = HIWORD(wParam);
					   WORD wLow = LOWORD(wParam);
					   if (1 == wHigh)
					   {
						   PrintDbgInfo(_T("响应到加速键消息, 加速键(alt + right)id:%d"), wLow);
					   }
					   else
					   {
						   if (IDC_BUTTON1 == wLow)
						   {
							   static bool bFlag = true;
							   MessageBox(hwndDlg, _T("在窗体中被处理了"), _T("提示"), MB_OK);
						   }
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
	default:
		break;
	}
	return 0;
}


INT_PTR CALLBACK BtnProc(
	_In_ HWND   hButton,// 按钮的
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
	)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	{
						   RECT rt;
						   GetClientRect(hButton, &rt);
						   WORD wX = GET_X_LPARAM(lParam);
						   WORD wY = GET_Y_LPARAM(lParam);
						   if (wX > rt.left && wX < rt.right && wY > rt.top && wY < rt.bottom)
						   {
							   MessageBox(hButton, TEXT("你在自己的回调函数里被处理了！"), 0, MB_OK);
							   HWND hParent = GetParent(hButton);
							   //获取自己的id
							   int nSelfCtrlID = GetDlgCtrlID(hButton);
							   //通知父窗口处理command消息
							   PostMessage(hParent, WM_COMMAND, MAKEWPARAM(nSelfCtrlID, BN_CLICKED), (LPARAM)hButton);
							   return 1;
						   }
	}
		break;
	default:
		break;
	}
	return g_oldProc(hButton, uMsg, wParam, lParam);
}
