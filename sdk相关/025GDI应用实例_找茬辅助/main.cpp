#include <windows.h>
#include <tchar.h>
#include <Windowsx.h>
#include "resource.h"
#include "Dbg.h"
#include <CommCtrl.h>
#include <vector>






#pragma comment(lib,"Comctl32.lib ")

INT_PTR CALLBACK DialogProc(
	_In_  HWND hwndDlg,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	);

//展示图片
void ShowPic(HDC hClientDc);
//找茬
void DoFindDiff(HWND hwndDlg);

HINSTANCE g_hInstance = NULL;
TCHAR g_szOrigTitle[MAX_PATH];
std::vector<POINT> g_vecFindPoint;

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
						  GetWindowText(hwndDlg, g_szOrigTitle, _countof(g_szOrigTitle));
						  return 1;
	}
		break;
	case WM_COMMAND:
	{
					   WORD wNotifyCode = HIWORD(wParam);
					   WORD wCtrlID = LOWORD(wParam);

					   switch (wCtrlID)
					   {
					   case ID_40001: //执行找茬
					   {
										 DoFindDiff(hwndDlg);
										 return 1;

					   }
					   case ID_40002: //恢复图片
					   {
										  //恢复标题
										  SetWindowText(hwndDlg, g_szOrigTitle);
										  //展示图片
										  g_vecFindPoint.clear();
										  HDC hClientDc = GetDC(hwndDlg);
										  ShowPic(hClientDc);
										  ReleaseDC(hwndDlg, hClientDc);
										  return 1;
					   }
						   break;
					   default:
						   break;
					   }
	}
		break;
	case WM_PAINT:
	{
					 PAINTSTRUCT ps;
					 HDC  hClientDc = BeginPaint(hwndDlg, &ps);
					 ShowPic(hClientDc);
					 EndPaint(hwndDlg, &ps);
					 return 1;
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

//展示图片
void ShowPic(HDC hClientDc)
{
	HDC hMemDc = CreateCompatibleDC(hClientDc);
	//
	HBITMAP hBitMap1 = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
	HBITMAP hBitMap2 = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
	BITMAP bitMap1;
	GetObject(hBitMap1, sizeof(BITMAP), &bitMap1);
	BITMAP bitMap2;
	GetObject(hBitMap2, sizeof(BITMAP), &bitMap2);
	//选入显示图片1
	SelectObject(hMemDc, hBitMap1);
	//显示上次查找的点
	for (size_t i = 0; i < g_vecFindPoint.size(); i++)
	{
		SetPixel(hMemDc, g_vecFindPoint[i].x, g_vecFindPoint[i].y, RGB(0xff, 0, 0));
	}
	BitBlt(hClientDc, 0, 0, bitMap1.bmWidth, bitMap1.bmHeight, hMemDc, 0, 0, SRCCOPY);
	//选入显示图片2
	SelectObject(hMemDc, hBitMap2);
	BitBlt(hClientDc, bitMap1.bmWidth + 10, 0, bitMap2.bmWidth, bitMap2.bmHeight, hMemDc, 0, 0, SRCCOPY);

	//
	DeleteObject(hBitMap2);
	DeleteObject(hBitMap1);
	//
	DeleteDC(hMemDc);
}

//找茬
void DoFindDiff(HWND hwndDlg)
{
	TCHAR szFindTitle[MAX_PATH];
	_stprintf_s(szFindTitle, _countof(szFindTitle), _T("正在查找中，请稍等......"));
	SetWindowText(hwndDlg, szFindTitle);
	g_vecFindPoint.clear();
	//
	HDC hClientDc = GetDC(hwndDlg);
	HBITMAP hBitMap1 = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
	HBITMAP hBitMap2 = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP2));

	//创建两个内存dc
	HDC hMemDc1 = CreateCompatibleDC(hClientDc);
	HDC hMemDc2 = CreateCompatibleDC(hClientDc);
	SelectObject(hMemDc1, hBitMap1);
	SelectObject(hMemDc2, hBitMap2);

	//获取图片信息
	BITMAP bitMap1;
	GetObject(hBitMap1, sizeof(BITMAP), &bitMap1);
	BITMAP bitMap2;
	GetObject(hBitMap2, sizeof(BITMAP), &bitMap2);

	//开始对比
	int nCount = 0;
	for (int i = 0; i < bitMap1.bmWidth; i++)
	{
		for (int j = 0; j < bitMap1.bmHeight; j++)
		{
			COLORREF colorBitmap1 = GetPixel(hMemDc1, i, j);
			COLORREF colorBitmap2 = GetPixel(hMemDc2, i, j);
			//
			if (colorBitmap1 != colorBitmap2)
			{
				++nCount;
				SetPixel(hMemDc1, i, j, RGB(0xff, 0, 0));
				POINT findedPoint = { i, j };
				g_vecFindPoint.push_back(findedPoint);
			}
		}
	}

	//显示对比结果
	BitBlt(hClientDc, 0, 0, bitMap1.bmWidth, bitMap1.bmHeight, hMemDc1, 0, 0, SRCCOPY);
	//删除图片
	DeleteObject(hBitMap2);
	DeleteObject(hBitMap1);
	DeleteDC(hMemDc2);
	DeleteDC(hMemDc1);
	ReleaseDC(hwndDlg, hClientDc);

	//
	if (nCount > 0)
	{
		TCHAR szAfterFindTitle[MAX_PATH];
		_stprintf_s(szAfterFindTitle, _countof(szAfterFindTitle), _T("查找的结果会用红色点标注出来"));
		SetWindowText(hwndDlg, szAfterFindTitle);
	}
	else
	{
		TCHAR szAfterFindTitle[MAX_PATH];
		_stprintf_s(szAfterFindTitle, _countof(szAfterFindTitle), _T("很遗憾没有找到不同的地方"));
		SetWindowText(hwndDlg, szAfterFindTitle);
	}
}
