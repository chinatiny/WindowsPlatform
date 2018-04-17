#include <windows.h>
#include <tchar.h>
#include <Windowsx.h>
#include "Dbg.h"
#include "resource.h"

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


//
void ShowNeedInfo(HWND hwndDlg, int nXScreenPos, int nYScreenPos)
{
	//准备工作
	HDC hDc = GetDC(hwndDlg);
	RECT clientRt;
	GetClientRect(hwndDlg, &clientRt);
	HBRUSH hEraseBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
	FillRect(hDc, &clientRt, hEraseBrush);
	//
	SetBkMode(hDc, TRANSPARENT);
	SetTextColor(hDc, RGB(0xff, 0, 0));
	//
	TextOut(hDc, 100, 80, _T("屏幕闪烁问题下面例子会解决"), _tcslen(_T("屏幕闪烁问题下面例子会解决")));

	//打印出客户区的坐标
	POINT cursorPos = {nXScreenPos, nYScreenPos};
	ScreenToClient(hwndDlg, &cursorPos);
	TCHAR szClientPos[MAX_PATH] = { 0 };
	_stprintf_s(szClientPos, _countof(szClientPos), _T("当前光标客户区坐标:(%d, %d)"), cursorPos.x, cursorPos.y);
	TextOut(hDc, 100, 100, szClientPos, _tcslen(szClientPos));


	//打印出屏幕坐标
	//ScreenToClient  => 屏幕坐标转化为客户区坐标
	TCHAR szScreenPos[MAX_PATH] = { 0 };
	_stprintf_s(szScreenPos, _countof(szScreenPos), _T("当前光标屏幕坐标:(%d, %d)"), nXScreenPos, nYScreenPos);
	TextOut(hDc, 100, 120, szScreenPos, _tcslen(szScreenPos));

	//收尾工作
	ReleaseDC(hwndDlg, hDc);
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
	case WM_COMMAND:
	{
					   WORD wNotifyCode = HIWORD(wParam);
					   WORD wCtrlID = LOWORD(wParam);
	}
		break;
	case WM_MOUSEMOVE:
	{
						// PrintWindowsMsgInfo(hwndDlg, uMsg, wParam, lParam);
						 int nXPos = GET_X_LPARAM(lParam);
						 int nYPos = GET_Y_LPARAM(lParam);

						 if (0 == wParam)
						 {
							 POINT pt = { nXPos, nYPos };
							 ClientToScreen(hwndDlg, &pt);
							 ShowNeedInfo(hwndDlg, pt.x, pt.y);
						 }
	}
		break;
	case WM_NCMOUSEMOVE:
	{
						   int nXPos = GET_X_LPARAM(lParam);
						   int nYPos = GET_Y_LPARAM(lParam);
						   ShowNeedInfo(hwndDlg, nXPos, nYPos);

	}
		break;
	case WM_CLOSE:
	{
					 PostQuitMessage(0);
					 return 1;
	}
		break;
	default:
		break;
	}
	return 0;
}
