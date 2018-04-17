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
HDC g_hClientDC = NULL;

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
						  g_hClientDC = GetDC(hwndDlg);
						  return 1;
	}
		break;
	case WM_COMMAND:
	{
					   WORD wNotifyCode = HIWORD(wParam);
					   WORD wCtrlID = LOWORD(wParam);

					   switch (wCtrlID)
					   {
					   case ID_40001: //矩形
					   {
										  HPEN hPen = CreatePen(PS_SOLID, 5, RGB(255, 0, 0));
										  SelectObject(g_hClientDC, hPen);
										  //透明画刷
										  HBRUSH hBsh = (HBRUSH)GetStockBrush(NULL_BRUSH);
										  SelectObject(g_hClientDC, hBsh);
										  Rectangle(g_hClientDC, 20, 20, 100, 100);
										  DeleteObject(hPen);
					   }
						   break;
					   case ID_40002: //椭圆
					   {
										  HPEN hPen = CreatePen(PS_SOLID, 5, RGB(255, 0, 0));
										  SelectObject(g_hClientDC, hPen);

										  //设置透明画刷
										  HBRUSH hBsh = (HBRUSH)GetStockBrush(NULL_BRUSH);
										  SelectObject(g_hClientDC, hBsh);

										  Ellipse(g_hClientDC, 20, 20, 100, 100);
										  DeleteObject(hPen);
					   }
						   break;
					   case ID_40003:  //多边形
					   {
										   HPEN hPen = CreatePen(PS_SOLID, 5, RGB(255, 0, 0));
										   SelectObject(g_hClientDC, hPen);

										   //设置透明画刷
										   HBRUSH hBsh = (HBRUSH)GetStockBrush(NULL_BRUSH);
										   SelectObject(g_hClientDC, hBsh);

										   POINT pt[] = { { 20, 20 }, { 50, 80 }, { 100, 0 }, { 120, 50 } };
										   Polygon(g_hClientDC, pt, 4);

										   DeleteObject(hPen);
					   }
						   break;
					   case ID_40004: //弧
					   {
										  Arc(g_hClientDC, 20, 20, 100, 100,// 弧必须落在该矩形内
											  12, 34, 56, 78);// 尽量靠近这两个点
					   }
						   break;
					   case ID_40005: //点
					   {
										  for (int i = 60; i < 100; i++)
										  {
											  for (int j = 60; j < 100; j++)
											  {
												  SetPixel(g_hClientDC, i, j, RGB(255, 0, 0));
											  }
										  }

					   }
						   break;
					   case ID_40006: //直线
					   {

										  HPEN hPen = CreatePen(PS_SOLID, 5, RGB(255, 0, 0));
										  HPEN hOldPen = (HPEN)SelectObject(g_hClientDC, hPen);
										  POINT pt = { 0 };
										  MoveToEx(g_hClientDC, 20, 20, &pt);
										  LineTo(g_hClientDC, 100, 100);
										  SelectObject(g_hClientDC, hOldPen);
										  DeleteObject(hPen);
					   }
						   break;
					   default:
						   break;
					   }
	}
		break;
	case WM_CLOSE:
	{
					 ReleaseDC(hwndDlg, g_hClientDC);
					 g_hClientDC = NULL;
					 PostQuitMessage(0);
					 return 1;
	}
		break;
	default:
		break;
	}
	return 0;
}
