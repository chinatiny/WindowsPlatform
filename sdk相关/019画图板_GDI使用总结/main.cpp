#include <windows.h>
#include <tchar.h>
#include <Windowsx.h>
#include "Dbg.h"
#include "resource.h"
#include <vector>

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

typedef enum 
{
	E_DRAW_LINE,
	E_DRAW_RETANGLE,
	E_DRAW_ELLIPSE,
	E_DRAW_MAX
}EDoWhat;

EDoWhat g_eDoWaht = E_DRAW_MAX;
POINT g_beginPoint = { 0 };

typedef struct
{
	EDoWhat eDoWhat;
	POINT beginPoint;
	POINT endPoint;
}ShapPoint;

std::vector<ShapPoint> g_vecShapPoint;

HDC g_hClientDC = NULL;
HDC g_hMemDc = NULL;
HBITMAP g_hMemMap = NULL;
HBRUSH g_hEraseBrush = NULL;

//
void DrawShapDetail(HDC hDc, int nXBeginPos, int nYBeginPos, int nXTargetPos, int nYTargetPos, EDoWhat eDowhat)
{
	//设置透明画刷
	HBRUSH hBsh = (HBRUSH)GetStockBrush(NULL_BRUSH);
	SelectObject(hDc, hBsh);
	//
	if (eDowhat == E_DRAW_LINE)
	{
		POINT pt = { 0 };
		MoveToEx(hDc, nXBeginPos, nYBeginPos, &pt);
		LineTo(hDc, nXTargetPos, nYTargetPos);
	}
	else if (eDowhat == E_DRAW_RETANGLE)
	{
		Rectangle(hDc, nXBeginPos, nYBeginPos, nXTargetPos, nYTargetPos);
	}
	else if (eDowhat == E_DRAW_ELLIPSE)
	{
		Ellipse(hDc, nXBeginPos, nYBeginPos, nXTargetPos, nYTargetPos);
	}
}

//执行画图
void DrawShap(HWND hwndDlg, int nXBeginPos, int nYBeginPos, int nXTargetPos, int nYTargetPos, EDoWhat eDowhat)
{
	//获取客户区
	RECT clientRt;
	GetClientRect(hwndDlg, &clientRt);
	//1. 清理掉旧的
 	FillRect(g_hMemDc, &clientRt, g_hEraseBrush);
	//InvalidateRect(hwndDlg, &clientRt, TRUE);

	//2. 画新的
	//恢复旧图像
	for (size_t i = 0; i < g_vecShapPoint.size(); i++)
	{
		DrawShapDetail(g_hMemDc,
			g_vecShapPoint[i].beginPoint.x,
			g_vecShapPoint[i].beginPoint.y, 
			g_vecShapPoint[i].endPoint.x,
			g_vecShapPoint[i].endPoint.y, 
			g_vecShapPoint[i].eDoWhat);
	}
	DrawShapDetail(g_hMemDc, nXBeginPos, nYBeginPos, nXTargetPos, nYTargetPos, eDowhat);

	//3. 贴图
	BitBlt(g_hClientDC, 0, 0, clientRt.right - clientRt.left, clientRt.bottom - clientRt.top, g_hMemDc, 0, 0, SRCCOPY);

}


// 在画线在移动的时候
void DoDrawLineOnMove(HWND hwndDlg, WPARAM wParam, LPARAM lParam)
{
	int nXPos = GET_X_LPARAM(lParam);
	int nYPos = GET_Y_LPARAM(lParam);
	PrintDbgInfo(_T("按住左键的坐标 画直线, x:%d, y:%d"), nXPos, nYPos);
	DrawShap(hwndDlg, g_beginPoint.x, g_beginPoint.y, nXPos, nYPos, g_eDoWaht);
}

//在画矩形在移动的时候
void DoDrawRectangleOnMove(HWND hwndDlg, WPARAM wParam, LPARAM lParam)
{
	int nXPos = GET_X_LPARAM(lParam);
	int nYPos = GET_Y_LPARAM(lParam);
	PrintDbgInfo(_T("按住左键的 画矩形 x:%d, y:%d"), nXPos, nYPos);
	DrawShap(hwndDlg, g_beginPoint.x, g_beginPoint.y, nXPos, nYPos, g_eDoWaht);
}


//在图椭圆移动的时候
void DoDrawEllipseOnMove(HWND hwndDlg, WPARAM wParam, LPARAM lParam)
{
	int nXPos = GET_X_LPARAM(lParam);
	int nYPos = GET_Y_LPARAM(lParam);
	PrintDbgInfo(_T("按住左键画圆 x:%d, y:%d"), nXPos, nYPos);
	DrawShap(hwndDlg, g_beginPoint.x, g_beginPoint.y, nXPos, nYPos, g_eDoWaht);
}

//在移动的时候
void DoDrawShapOnMove(HWND hwndDlg, WPARAM wParam, LPARAM lParam)
{

	switch (g_eDoWaht)
	{
	case E_DRAW_LINE:
		DoDrawLineOnMove(hwndDlg, wParam, lParam);
		break;
	case E_DRAW_RETANGLE:
		DoDrawRectangleOnMove(hwndDlg, wParam, lParam);
		break;
	case E_DRAW_ELLIPSE:
		DoDrawEllipseOnMove(hwndDlg, wParam, lParam);
		break;
	default:
		break;
	}

}

//在左键抬起的时候
void DoDrawShapOnButtonUp(HWND hwndDlg, WPARAM wParam, LPARAM lParam)
{
	//保存数据
	int nXPos = GET_X_LPARAM(lParam);
	int nYPos = GET_Y_LPARAM(lParam);
	POINT beginPoint = { g_beginPoint.x, g_beginPoint.y };
	POINT endPoint = { nXPos, nYPos };
	//
	ShapPoint shapPoint;
	shapPoint.beginPoint = beginPoint;
	shapPoint.endPoint = endPoint;
	shapPoint.eDoWhat = g_eDoWaht;
	g_vecShapPoint.push_back(shapPoint);

	//清理掉旧数据
	//g_eDoWaht = E_DRAW_MAX;
	g_beginPoint.x = 0;
	g_beginPoint.y = 0;
	PrintDbgInfo(_T("弹起左键，设置为无效动作状态"));
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
						  g_hMemDc = CreateCompatibleDC(g_hClientDC);
						  //获取客户区
						  RECT clientRt;
						  GetClientRect(hwndDlg, &clientRt);
						  // 给内存DC提供画布
						  HBITMAP hMemMap = CreateCompatibleBitmap(g_hClientDC, clientRt.right, clientRt.bottom);
						  //设置画布
						  SelectObject(g_hMemDc, hMemMap);
						  //设置清理画刷
						  g_hEraseBrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
						  SelectObject(g_hMemDc, g_hEraseBrush);
	}
		break;
	case WM_COMMAND:
	{
					   WORD wNotifyCode = HIWORD(wParam);
					   WORD wCtrlID = LOWORD(wParam);
					   switch (wCtrlID)
					   {
					   case ID_40001:  //直线
					   {
										   g_eDoWaht = E_DRAW_LINE;
					   }
						   break;
					   case ID_40002: //圆
					   {
										  g_eDoWaht = E_DRAW_ELLIPSE;
					   }
						   break;
					   case ID_40003://矩形
					   {
										 g_eDoWaht = E_DRAW_RETANGLE;
					   }
						   break;
					   default:
						   break;
					   }
	}
		break;

	case WM_MOUSEMOVE:
	{
						 if (wParam & MK_LBUTTON)
						 {
							 if (g_eDoWaht != E_DRAW_MAX)
								DoDrawShapOnMove(hwndDlg, wParam, lParam);
							 return 1;
						 }
	}
		break;
	case WM_LBUTTONUP:
	{
						 if (g_eDoWaht != E_DRAW_MAX)
							 DoDrawShapOnButtonUp(hwndDlg, wParam, lParam);
	}
		break;
	case WM_LBUTTONDOWN:
	{
						   if (wParam & MK_LBUTTON)
						   {
							   if (g_eDoWaht != E_DRAW_MAX)
							   {
								   int nXPos = GET_X_LPARAM(lParam);
								   int nYPos = GET_Y_LPARAM(lParam);
								   PrintDbgInfo(_T("选取的开始点, x:%d, y:%d"), nXPos, nYPos);
								   g_beginPoint.x = nXPos;
								   g_beginPoint.y = nYPos;
							   }

							   return 1;
						   }

	}
		break;
	case WM_PAINT:
	{
					 PAINTSTRUCT ps = { 0 };
					 HDC hDc = BeginPaint(hwndDlg, &ps);

					 for (size_t i = 0; i < g_vecShapPoint.size(); i++)
					 {
						 DrawShapDetail(hDc,
							 g_vecShapPoint[i].beginPoint.x,
							 g_vecShapPoint[i].beginPoint.y,
							 g_vecShapPoint[i].endPoint.x,
							 g_vecShapPoint[i].endPoint.y,
							 g_vecShapPoint[i].eDoWhat);
					 }

					 EndPaint(hwndDlg, &ps);
	}
		break;
	case WM_CLOSE:
	{
					 //4. 善后工作
					 DeleteBrush(g_hEraseBrush);
					 DeleteDC(g_hMemDc);
					 ReleaseDC(hwndDlg, g_hClientDC);
					 PostQuitMessage(0);
					 return 1;
	}
		break;
	default:
		break;
	}
	return 0;
}
