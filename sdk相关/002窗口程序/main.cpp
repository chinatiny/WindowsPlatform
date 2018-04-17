#include <windows.h>
#include <tchar.h>
#include "Dbg.h"

LRESULT CALLBACK WindowProc(
	_In_ HWND   hwnd,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
	);


//////////////////////////////////////////////
HINSTANCE g_hIntance = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	CreateDbgConsole();
	g_hIntance = hInstance;
	//1. 构建窗体类
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;

	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);

	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = _T("第一个窗口类");

	//2. 注册窗口
	RegisterClassEx(&wcex);

	//3.创建窗口
	HWND hWnd = CreateWindow(_T("第一个窗口类"),
		_T("我的第一个窗口"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
		NULL,
		NULL,
		hInstance,
		NULL);

	//4. 显示刷新窗口
	ShowWindow(hWnd, nShowCmd);
	UpdateWindow(hWnd);

	//5.消息循环
	MSG msg = { 0 };
	while (GetMessage(&msg, 0, 0, 0))
	{
		//
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//6. 退出循环
	return msg.wParam;
}

LRESULT CALLBACK WindowProc(
	_In_ HWND   hwnd,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
	)
{
	/*
	1. 自己的消息
	2. 子控件的消息 WM_COMMAND(简单控件 按钮或菜单) WM_NOTIFY(list tree等复杂控件)
	*/
	//利用调试函数打印出信息
	PrintWindowsMsgInfo(hwnd, uMsg, wParam, lParam);
	switch (uMsg)
	{
	case WM_CREATE:
	{
					  //窗口程序创建消息顺序:
					  //WM_CREATE -> WM_SHOW
					  CreateWindow(_T("BUTTON"), _T("我的第一个按钮"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 100, 30, hwnd, (HMENU)0X100, g_hIntance, NULL);
					  CreateDbgConsole();
	}
		break;
	case WM_QUIT:  //不可能在回调函数中接收到
	{
					   PrintDbgInfo(_T("uMsg:0x%X    wParam:0x%X  lParam:0x%X\n"), uMsg, wParam, lParam);
	}
		break;
	case WM_CLOSE:
	{
					 //程序结束流程:
					 //WM_CLOSE -> WM_DESTORY -> WM_QUIT(WM_QUIT不会被接收到)
					 PostQuitMessage(0);
	}
		break;

	case WM_COMMAND:
	{
					   //The LOWORD contains the button's control identifier. The HIWORD specifies the notification code.
					   WORD wCtlID = LOWORD(wParam);
					   WORD wNotifyCode = HIWORD(wParam);
					   switch (wNotifyCode)
					   {
					   case BN_CLICKED:
					   {
										  switch (wCtlID)
										  {
										  case 0X100:
										  {
														MessageBox(hwnd, _T("第一个按钮"), _T("提示"), MB_OK);
										  }
											  break;
										  default:
											  break;
										  }
					   }
						   break;
					   default:
						   break;
					   }

	}
		break;
	case WM_PAINT:
	{
					 PAINTSTRUCT ps = { 0 };
					 HDC hDc = BeginPaint(hwnd, &ps);
					 TextOut(hDc, 100, 100, _T("15pb"), 4);
					 EndPaint(hwnd, &ps);

	}
		break;

	default:
		break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}