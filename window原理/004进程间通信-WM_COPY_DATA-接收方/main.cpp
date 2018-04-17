#include <windows.h>
#include <tchar.h>

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

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;

	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);

	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = _T("copy data 测试");

	//2. 注册窗口
	RegisterClassEx(&wcex);

	//3.创建窗口
	HWND hWnd = CreateWindow(_T("copy data 测试"),
		_T("copy data 测试"),
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
	switch (uMsg)
	{
	case WM_CLOSE:
	{
					 //程序结束流程:
					 //WM_CLOSE -> WM_DESTORY -> WM_QUIT(WM_QUIT不会被接收到)
					 PostQuitMessage(0);
	}
	case WM_COPYDATA:
	{
						COPYDATASTRUCT *pCopyData = (COPYDATASTRUCT*)lParam;
						MessageBox(hwnd, (TCHAR*)pCopyData->lpData, _T("来自WM_COPYDATA"), MB_OK);
	}
		break;

	default:
		break;
	}


	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}