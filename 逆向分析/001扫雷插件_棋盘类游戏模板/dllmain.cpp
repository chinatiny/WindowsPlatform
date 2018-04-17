#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include "Dbg.h"


HWND s_hWnd = NULL;
WNDPROC s_OldProc = NULL;

BYTE* s_pbyMineArrayBase = (BYTE*)0x1005340;
int* s_pWidth = (int*)0x1005334;
int* s_pHeight = (int*)0x1005338;
int* s_pMine = (int*)0x1005330;


LRESULT WINAPI MyWindowProc(
	_In_ HWND hWnd,
	_In_ UINT Msg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam) {

	// 1. 做自己的事
	//DefWindowProc
	if (Msg == WM_KEYDOWN && wParam == VK_F5) 
	{
		OutputDebugString(L" VK_F5 ");

		//01002EE4 | MOV ECX, DWORD PTR DS : [<宽度>];  X
		//01002EEA | MOV EDX, DWORD PTR DS : [<高度>];  Y
		//01002EF0 | LEA EAX, DWORD PTR DS : [ECX + 0x2];  eax = x + 2
		//01002EF3 | TEST EAX, EAX
		//01002EF5 | PUSH ESI;  保存寄存器环境
		//01002EF6 | JE SHORT winmine.01002F11

		//01002EF8 | MOV ESI, EDX;  esi = Y
		//01002EFA | SHL ESI, 0x5;  esi = y * 32
		//01002EFD | LEA ESI, DWORD PTR DS : [ESI + 0x1005360];  esi = y * 32 + 数组基地址 + 32
		//01002F03 | DEC EAX;  横坐标递减，遍历X，0 - x + 1
		//01002F04 | MOV BYTE PTR DS : [EAX + <数组基地址>], 0x10
		//01002F0B | MOV BYTE PTR DS : [ESI + EAX], 0x10
		//01002F0F | JNZ SHORT winmine.01002F03

		int nWidth = *s_pWidth, nHeight = *s_pHeight;
		int nMineCount = *s_pMine;
		int nCount = 0;

		for (int y = 1; y < nHeight + 1; y++)
		{
			TCHAR strLine[MAX_PATH] = { 0 };
			for (int x = 1; x < nWidth + 1; x++)
			{
				BYTE* pAddr = y * 32 + s_pbyMineArrayBase;
				BYTE byCode = *(BYTE*)(pAddr + x);
				if (byCode == (BYTE)0x8F) 
				{
					nCount++;
				}
				else
				{
					int xPos, yPos;
					xPos = (x << 4) - 4;
					yPos = (y << 4) + 0x27;
					SendMessage(hWnd, WM_LBUTTONDOWN, 0, MAKELPARAM(xPos, yPos));
					SendMessage(hWnd, WM_LBUTTONUP, 0, MAKELPARAM(xPos, yPos));
				}

				TCHAR strCode[MAX_PATH] = { 0 };
				_stprintf_s(strCode, _countof(strCode), _T("%02x "), byCode);
				_stprintf_s(strLine, _countof(strLine), _T("%s%02x "), strLine, byCode);
			}
			_stprintf_s(strLine, _countof(strLine), _T("%s\r\n"), strLine);
			PrintDbgInfo(_T("%s"), strLine);
		}
		TCHAR strCount[MAX_PATH] = { 0 };
		_stprintf_s(strCount, _countof(strCount), _T("雷数量:%d"), nCount);
		PrintDbgInfo(_T("%s"), strCount);
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	else if (Msg == WM_MOUSEMOVE) 
	{
		//01002099 | .  8B45 14       MOV EAX, [ARG.4];  11111
		//0100209C | .C1E8 10       SHR EAX, 0x10;  y
		//0100209F | .  83E8 27       SUB EAX, 0x27;  y - 0x27
		//010020A2 | .C1F8 04       SAR EAX, 0x4;  (y - 0x27) >> 4
		//010020A5 | .  50            PUSH EAX
		//010020A6 | .  0FB745 14     MOVZX EAX, WORD PTR SS : [EBP + 0x14];  x
		//010020AA | .  83C0 04       ADD EAX, 0x4;  x + 4
		//010020AD | .C1F8 04       SAR EAX, 0x4;  (x + 4) >> 4
		//010020B0 | .  50            PUSH EAX
		//010020B1 | >  E8 1E110000   CALL winmine.010031D4
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		int x = (xPos + 4) >> 4;
		int y = (yPos - 0x27) >> 4;

		BYTE* pAddr = y * 32 + s_pbyMineArrayBase;
		BYTE byCode = *(BYTE*)(pAddr + x);
		if (byCode == (BYTE)0x8F)
		{
			SetWindowText(hWnd, _T("此处有雷，小心为妙"));
		}
		else
		{
			SetWindowText(hWnd, _T("扫雷【一个其乐无穷的游戏】"));
		}
	}


	// 2. 不是自己的消息，继续调用原窗口回调函数
	return CallWindowProc(s_OldProc, hWnd, Msg, wParam, lParam);
}


void Init()
{
	CreateDbgConsole();
	s_hWnd = FindWindow(NULL, _T("扫雷"));
	if (NULL == s_hWnd) 
	{
		PrintDbgInfo(_T("没有找到扫描雷程序"));
	}
	// 2. 修改窗口回调函数
	s_OldProc = (WNDPROC)SetWindowLong(
		s_hWnd,
		GWLP_WNDPROC, (LONG)MyWindowProc);

	PrintDbgInfo(_T(" 注入成功，窗口已修改 g_oldproc:%08X"), s_OldProc);
}

void Destory()
{
	// 恢复窗口回调
	SetWindowLong(s_hWnd,
		GWLP_WNDPROC,
		(LONG)s_OldProc);
}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Init();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		Destory();
		break;
	}
	return TRUE;
}

