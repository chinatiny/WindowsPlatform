#include <windows.h>
#include <tchar.h>
#include   <stdarg.h>  
#include <Shlwapi.h>
#pragma  comment(lib, "Shlwapi.lib")

LRESULT CALLBACK WindowProc(
	_In_ HWND   hwnd,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
	);

BOOL CALLBACK EnumChildProc(
	_In_ HWND   hwnd,
	_In_ LPARAM lParam
	);

void PrintDbgInfo(LPCTSTR lptstrFormat, ...);

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
	switch (uMsg)
	{
	case WM_CREATE:
	{
					  //窗口程序创建消息顺序:
					  //WM_CREATE -> WM_SHOW
					  CreateWindow(_T("BUTTON"), _T("我的第一个按钮"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 0, 100, 30, hwnd, (HMENU)0X100, g_hIntance, NULL);
					  CreateWindow(_T("BUTTON"), _T("我的第二个按钮"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 31, 100, 30, hwnd, (HMENU)0X101, g_hIntance, NULL);
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
										  case 0x101:
										  {
														//第一种：适合查找顶层窗口句柄
														HWND hCalc = FindWindow(NULL,_T("计算器"));
														if (hCalc == NULL)
														{
															WinExec("calc", SW_SHOW);
															Sleep(500);
															hCalc = FindWindow(NULL, _T("计算器"));
														}
														PrintDbgInfo(_T("计算器窗体句柄：0x%X"), hCalc);

														//第二种：已知一个窗体句柄，查找相关的的句柄
														HWND hChild = GetWindow(hCalc, GW_CHILD);
														PrintDbgInfo(_T("计算器的相关句柄：0x%X"), hChild);

														//第三种：已知一个句柄，频繁调用GetWindow的时候
														HWND hFind = 0;
														EnumChildWindows(hChild, EnumChildProc, (LPARAM)&hFind);

														//第四种：已知父窗口的句柄，通过控件id获取句柄
														HWND hButton1 = GetDlgItem(hwnd, 0X100);
														PrintDbgInfo(_T("按钮1的句柄：0x%X"), hButton1);
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


BOOL CALLBACK EnumChildProc(
	_In_ HWND   hwnd,
	_In_ LPARAM lParam
)
{
	TCHAR szTextBuff[MAX_PATH];
	TCHAR szClassNameBuff[MAX_PATH];
	GetWindowText(hwnd, szTextBuff, _countof(szTextBuff) - 1);
	GetClassName(hwnd, szClassNameBuff, _countof(szClassNameBuff) - 1);
	PrintDbgInfo(_T("窗体的句柄：0x%X 窗体的类名：%s  窗体的名称: %s"), hwnd, szClassNameBuff, szTextBuff);
	return true;
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 多字符转换为宽字符(Unicode  --> ASCII)
#define  WCHAR_TO_CHAR(lpW_Char, lpChar) \
	WideCharToMultiByte(CP_ACP, NULL, lpW_Char, -1, lpChar, _countof(lpChar), NULL, FALSE);

// 多字符转换为宽字符(ASCII --> Unicode)
#define  CHAR_TO_WCHAR(lpChar, lpW_Char) \
	MultiByteToWideChar(CP_ACP, NULL, lpChar, -1, lpW_Char, _countof(lpW_Char));

//格式化输出信息字符串到输出缓冲区
void SprintfDbgInfo(TCHAR *pOutBuff, int nMaxOutBuffSize, LPCTSTR lptstrFormat, va_list argList)
{
	//
	const int nFileBuffSize = 256;
	TCHAR fileNameBuff[nFileBuffSize];

	//设置文件名和行号
#ifdef UNICODE
	CHAR_TO_WCHAR(__FILE__, fileNameBuff);
	TCHAR *pFileName = PathFindFileName(fileNameBuff);
	_stprintf_s(pOutBuff, nMaxOutBuffSize - sizeof(TCHAR), _T("[mod:%s:line:%d]  ::  "), pFileName, __LINE__);
#else
	_tcscpy_s(fileNameBuff, __FILE__);
	TCHAR *pFileName = PathFindFileName(fileNameBuff);
	_stprintf_s(pOutBuff, nMaxOutBuffSize - sizeof(TCHAR), _T("[mod:%s:line:%d]  ::"), pFileName, __LINE__);
#endif

	//设置输入信息
	int nHeadLen = _tcslen(pOutBuff);
	_vstprintf_s(pOutBuff + nHeadLen, nMaxOutBuffSize - sizeof(TCHAR)-nHeadLen * sizeof(TCHAR), lptstrFormat, argList);

	//设置换行
	int nBodyLen = _tcslen(pOutBuff);
	_stprintf_s(pOutBuff + nBodyLen, nMaxOutBuffSize - sizeof(TCHAR)-nBodyLen * sizeof(TCHAR), _T("\r\n"));
}

//打印出调试信息
void PrintDbgInfo(LPCTSTR lptstrFormat, ...)
{
	const int c_nMaxBuffSize = 4096;
	TCHAR outBuff[c_nMaxBuffSize];

	//把结果输出到缓冲区
	va_list argList;
	va_start(argList, lptstrFormat);
	SprintfDbgInfo(outBuff, c_nMaxBuffSize, lptstrFormat, argList);
	va_end(argList);

	//输出信息
	OutputDebugString(outBuff);
}
