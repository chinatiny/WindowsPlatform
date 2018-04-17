#include <tchar.h>
#include <windows.h>
#include "MsgHookInterface.h"

HHOOK	g_hHook;
HMODULE	g_hModule;



BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
							   g_hModule = hModule;
	}

		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}





LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam) {

	// 判断是否wParam与lParam都有键盘消息，
	//是的话则执行打印操作
	if (code == HC_ACTION) {

		// 将256个虚拟键的状态拷贝到指定的缓冲区中，
		//如果成功则继续
		BYTE KeyState[256] = { 0 };
		if (GetKeyboardState(KeyState)) {


			// 得到第16–23位，键盘虚拟码
			UINT  keyCode = (lParam >> 16) & 0x00ff;
			WCHAR wKeyCode = 0;
			// 转换成ASCII码
			ToAscii((UINT)wParam, keyCode, KeyState, (LPWORD)&wKeyCode, 0);


			// 获取前端窗口
			TCHAR szWndTitle[512];
			HWND hWnd = GetForegroundWindow();
			// 获取窗口标题.
			GetWindowText(hWnd, szWndTitle, 512);


			// 将其打印出来
			WCHAR szInfo[516] = { 0 };
			swprintf_s(szInfo, _countof(szInfo), L">>>>   [%s] : %c <<<<", szWndTitle, (WCHAR)wKeyCode);

			OutputDebugString(szInfo);
		}
	}

	return CallNextHookEx(g_hHook, code, wParam, lParam);
}
MESSAGEHOOKDLL_API DWORD g_tid = 0;

// 安装钩子
MESSAGEHOOKDLL_API int InstallHook() {

	g_hHook = SetWindowsHookEx(
		WH_KEYBOARD,				/* 要HOOK的消息类型 */
		KeyboardProc,				/* 钩子回调 */
		g_hModule,					/* 钩子回调函数所在的模块句柄,在这里使用的是本DLL的句柄 */
		g_tid						/* 要钩住的线程ID,为0时钩住所有线程 */
		);
	return g_hHook != NULL;
}


// 卸载钩子
MESSAGEHOOKDLL_API int UninstallHook() 
{

	return UnhookWindowsHookEx(g_hHook);
}

