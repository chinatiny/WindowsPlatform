#include "ImmAnti.h"
#include <windows.h>
#include <imm.h>

//////////////////////////////内部接口///////////////////////////////////////
HHOOK s_hHook = NULL;
DWORD s_dwThreadID = 0;


//消息处理函数
LRESULT CALLBACK MsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PMSG pmsg = (PMSG)lParam;

	if (pmsg->message == WM_IME_NOTIFY)
	{
		//IMN_CHANGECANDIDATE这个消息，输入法选项的变化
		if (pmsg->wParam & IMN_CHANGECANDIDATE)
		{
			GUITHREADINFO gui;
			gui.cbSize = sizeof(gui);
			GetGUIThreadInfo(s_dwThreadID, &gui);
		}
	}

	return CallNextHookEx(s_hHook, nCode, wParam, lParam);
}


//////////////////////////////对外接口///////////////////////////////////////
void ImmMonitor()
{
	DWORD dwSelfThradID = GetCurrentThreadId();
	s_dwThreadID = dwSelfThradID;
	s_hHook = SetWindowsHookEx(
		WH_GETMESSAGE,
		MsgProc,
		GetModuleHandle(NULL),	
		dwSelfThradID
		);
}
