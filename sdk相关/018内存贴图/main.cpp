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


INT_PTR CALLBACK DialogProc(
	_In_  HWND hwndDlg,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	)
{
	PrintWindowsMsgInfo(hwndDlg, uMsg, wParam, lParam);
	switch (uMsg)
	{
	case WM_COMMAND:
	{
					   WORD wNotifyCode = HIWORD(wParam);
					   WORD wCtrlID = LOWORD(wParam);
					   switch (wCtrlID)
					   {
					   case ID_40001:  //执行无拉伸拷贝贴图
					   {
										//准备工作
										HDC hClientDC = GetDC(hwndDlg);
										HDC hMemDc = CreateCompatibleDC(hClientDC);

										//载入图片
										HBITMAP hBmp = (HBITMAP)LoadImage(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
										SelectObject(hMemDc, hBmp);
										//无拉伸贴图
										BitBlt(hClientDC, 100, 100, 500, 500, hMemDc, 0, 0, SRCCOPY);

										//删除资源
										DeleteObject(hBmp);

										//清理工作
										DeleteDC(hMemDc);
										ReleaseDC(hwndDlg, hClientDC);

					   }
						   break;
					   case ID_40002: //执行无拉伸异或者贴图
					   {
										  //准备工作
										  HDC hClientDC = GetDC(hwndDlg);
										  HDC hMemDc = CreateCompatibleDC(hClientDC);

										  //载入图片
										  HBITMAP hBmp = (HBITMAP)LoadImage(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
										  SelectObject(hMemDc, hBmp);
										  //无拉伸贴图
										  BitBlt(hClientDC, 100, 100, 500, 500, hMemDc, 0, 0, SRCINVERT);

										  //删除资源
										  DeleteObject(hBmp);

										  //清理工作
										  DeleteDC(hMemDc);
										  ReleaseDC(hwndDlg, hClientDC);

					   }
						   break;
					   case ID_40003://执行拉伸拷贝贴图
					   {
										 RECT clientRt;
										 GetClientRect(hwndDlg, &clientRt);

										 //准备工作
										 HDC hClientDC = GetDC(hwndDlg);
										 HDC hMemDc = CreateCompatibleDC(hClientDC);

										 //载入图片
										 HBITMAP hBmp = (HBITMAP)LoadImage(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
										 SelectObject(hMemDc, hBmp);
										 //拉伸贴图
										 StretchBlt(hClientDC, 0, 0, clientRt.right, clientRt.bottom, hMemDc, 0, 0, 656, 331, SRCCOPY);

										 //删除资源
										 DeleteObject(hBmp);

										 //清理工作
										 DeleteDC(hMemDc);
										 ReleaseDC(hwndDlg, hClientDC);

					   }
						   break;
					   case ID_40004: //执行拉伸异或者贴图
					   {
										  RECT clientRt;
										  GetClientRect(hwndDlg, &clientRt);

										  //准备工作
										  HDC hClientDC = GetDC(hwndDlg);
										  HDC hMemDc = CreateCompatibleDC(hClientDC);

										  //载入图片
										  HBITMAP hBmp = (HBITMAP)LoadImage(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
										  SelectObject(hMemDc, hBmp);
										  //拉伸贴图
										  StretchBlt(hClientDC, 0, 0, clientRt.right, clientRt.bottom, hMemDc, 0, 0, 656, 331, SRCINVERT);

										  //删除资源
										  DeleteObject(hBmp);

										  //清理工作
										  DeleteDC(hMemDc);
										  ReleaseDC(hwndDlg, hClientDC);

					   }
						   break;
					   case ID_40005: //还原贴图原来的样子
					   {
										  HDC hClientDc = GetDC(hwndDlg);
										  RECT rt;
										  GetClientRect(hwndDlg, &rt);
 										  HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
										  FillRect(hClientDc, &rt, hBrush);
										  DeleteBrush(hBrush);
										  ReleaseDC(hwndDlg, hClientDc);
					   }
						   break;
					   default:
						   break;
					   }
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
