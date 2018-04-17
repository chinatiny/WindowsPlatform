#include <windows.h>
#include <tchar.h>
#include "Dbg.h"
#include "resource.h"

/*
	一：窗口重绘时机：
	1. 改变窗口大小改变，  窗口被覆盖重新显示
	2. 窗口有区域需要重新绘制  => InvalidateRect

	二：WM_PAINT消息低优先级、合并性

	三：被覆盖的窗口需要覆盖者来重绘(自己总结得出来的，发现被覆盖的窗口如果没有获取到焦点是不会触发WM_PAINT的)

	四：关于BeginPaint和EndPaint
	1. BeginPaint可以使无效区域变得有效，让系统不再发送WM_PAINT消息
	2. 执行绘制并且释放dc
	3. 如果在WM_PAINT消息中去掉BeginPaint和EndPaint，会导致无效区域一直无效，系统不断的发送paint消息会导致程序卡死
'*/


/*
	获取dc的方式：===>下面通过按钮消息会一个个实现
	1.  BeginPaint获取 EndPaint释放->只能用在WM_PAINT消息中
	2.  GetDC获取ReleaseDC释放->任何地方都可以 获取到,缺点是只能获取客户区的dc，不能绘制标题栏
	3.  GetWindowDC & ReleaseDC->任何地方都可以获取，可以绘制标题栏， 需要处理WM_NCPAINT消息，不然会被覆盖
	4. CreateDC获取DeleteDC-> 可以获取别的创面程序的dc
*/

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
	g_hIntance = hInstance;
	

	//1. 构建窗体类
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
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

	case WM_NCPAINT:  //GetWindowDC & ReleaseDC->任何地方都可以获取，可以绘制标题
	{
					   return 1;
	}
		break;

	case WM_COMMAND:
	{
					   //The LOWORD contains the button's control identifier. The HIWORD specifies the notification code.
					   WORD wCtlID = LOWORD(wParam);
					   WORD wNotifyCode = HIWORD(wParam);

					   switch (wCtlID)
					   {
					   case ID_40002:  //制定无效区域
					   {
										   RECT rect;
										   GetClientRect(hwnd, &rect);
										   //让整个客户区都无效
										   InvalidateRect(hwnd, &rect, FALSE);
					   }
						   break;
					   case ID_40003:  //GetDC获取ReleaseDC释放->任何地方都可以 获取到, 缺点是只能获取客户区的dc，不能绘制标题栏
					   {
										   HDC hDc = GetDC(hwnd);
										   TextOut(hDc, 0, 0, _T("这个是getdc"), _tcslen(_T("这个是getdc")));
										   ReleaseDC(hwnd, hDc);
					   }
						   break;
					   case ID_40004:  // GetWindowDC & ReleaseDC->任何地方都可以获取，可以绘制标题, 需要处理WM_NCPAINT消息
					   {
										   PrintDbgInfo(_T("get window dc"));
										   HDC hDc =  GetWindowDC(hwnd);
										   TextOut(hDc, 0, 0, _T("这个是GetWindowDC"), _tcslen(_T("这个是GetWindowDC")));
										   ReleaseDC(hwnd, hDc);
					   }
						   break;
					   case ID_40005: //CreateDC获取DeleteDC-> 可以获取别的创面程序的dc
					   {
										  // 桌面DC
										  HDC hDesktop = CreateDC(L"DISPLAY", NULL, NULL, NULL);
										  TextOut(hDesktop, 0, 0, _T("这个是CreateDC"), _tcslen(_T("这个是CreateDC")));
										  DeleteDC(hDesktop);
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

					 //设置客户区的背景颜色
					 HBRUSH hBsh = CreateSolidBrush(RGB(0, 255, 0));
					 RECT rt;
					 GetClientRect(hwnd, &rt);
					 FillRect(hDc, &rt, hBsh);

					 TextOut(hDc, 100, 100, _T("在设置背景文字背景色子之前"), _tcslen(_T("在设置背景文字背景色之前")));
					 //设置输出文字的背景色为透明，不然很难看
					 SetBkMode(hDc, TRANSPARENT);
					 //输出文字
					 SetTextColor(hDc, RGB(0xff, 0, 0));
					 TextOut(hDc, 100, 120, _T("在设置背景文字背景色子之后"), _tcslen(_T("在设置背景文字背景色之后")));
					 EndPaint(hwnd, &ps);

	}
		break;

	default:
		break;
	}


	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}