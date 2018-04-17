//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 如果盾牌没有出现在按钮, 可以加入以下宏.
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////



#include "resource.h"
#include <shlobj.h>

BOOL    IsAdmin(HANDLE hProcess);

INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL    IsAdmin(HANDLE hProcess);


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
}

INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch (uMsg) {

	case WM_INITDIALOG:
	{
						  HWND hButton = GetDlgItem(hWnd, BTN_RunAsAdmin);

						  // 判断当前是否是以管理员身份运行的进程.
						  if (!IsAdmin(GetCurrentProcess())) {

							  // 如果不是, 则将一个按钮设置为带盾牌标志的按钮.
							  Button_SetElevationRequiredState(hButton, TRUE);
						  }
						  else {

							  // 如果已经以管理员身份运行,则将按钮隐藏
							  ShowWindow(hButton, SW_HIDE);
						  }
	}
		break;

	case WM_COMMAND:
	{
					   switch (LOWORD(wParam)) {

					   case BTN_RunAsAdmin:  // 以管理员身份运行的按钮被点击事件
					   {
												 // 1. 获取本进程路径
												 TCHAR path[MAX_PATH] = { 0 };
												 DWORD dwSize = MAX_PATH;
												 QueryFullProcessImageName(GetCurrentProcess(), 0, path, &dwSize);

												 // 隐藏窗口
												 ShowWindow(hWnd, SW_HIDE);

												 // 2. 以管理员身份运行
												 ShellExecute(hWnd,
													 L"runas", /*系统命令，意思以管理运行一个进程*/
													 path,
													 NULL, NULL, SW_SHOW);

												 if (ERROR_SUCCESS == GetLastError()) {
													 // 如果创建成功则退出进程.
													 ExitProcess(0);
												 }
												 else {
													 // 如果创建进程失败, 则将窗口显示回来
													 ShowWindow(hWnd, SW_SHOW);
												 }
					   }
						   break;
					   }
	}
		break;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;
	default:return FALSE;
		break;
	}

	return TRUE;
}



BOOL    IsAdmin(HANDLE hProcess)
{
	HANDLE hToken = NULL;
	OpenProcessToken(hProcess, TOKEN_QUERY, &hToken);

	TOKEN_ELEVATION_TYPE tokenType = TokenElevationTypeDefault; // 用于接收令牌类型

	DWORD dwRetSize = 0; // 用于接收函数输出信息的字节数
	// 2. 查询进程令牌中的权限提升值.( 这个值会记录当前的令牌是何种类型( 细节将17_权限管理_令牌的获取.cpp ) )
	GetTokenInformation(hToken,
		TokenElevationType,// 获取令牌的当前提升等级
		&tokenType,
		sizeof(tokenType),
		&dwRetSize // 所需缓冲区的字节数
		);

	// 根据令牌的类型来输出相应的信息
	if (TokenElevationTypeFull == tokenType) {
		// 3. 如果令牌是TokenElevationTypeFull , 则拥有至高无上的能力,可以给令牌添加任何特权,返回第0步执行代码.
		return TRUE;
	}
	// 4. 如果是其他的, 则需要以管理员身份重新运行本进程. 这样就能以第三步的方法解决剩下的问题.
	else if (TokenElevationTypeDefault == tokenType) {

		// 默认用户可能是一个没有权限的标准用户
		// 也可能是UAC被关闭.
		// 直接调用系统的函数IsUserAnAdmin 来判断用户是否是管理员.
		return IsUserAnAdmin();
	}
	else if (TokenElevationTypeLimited == tokenType) {

		return FALSE;
	}

	return FALSE;
}

