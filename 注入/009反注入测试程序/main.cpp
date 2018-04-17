#include <windows.h>
#include <tchar.h>
#include "Dbg.h"
#include "ThreadAnti.h"
#include "LibaryAnti.h"
#include "ProcessAnti.h"
#include "PEAnti.h"
#include "ApcAnti.h"
#include "RegAnti.h"
#include "ImmAnti.h"
#include "AttachAnti.h"
#include "resource.h"

#define  LOOP_SEC  1000

DWORD WINAPI TestThreadProc(
	_In_  LPVOID lpParameter
	)
{
	__asm int 3
	PrintDbgInfo(_T("_____________"));
	return 1;
}

DWORD WINAPI ApcThreadProc(
	_In_  LPVOID lpParameter
	)
{
	while (true)
	{
		PrintDbgInfo(_T("AAAAAAAAAAAAAAAA"));
		SleepEx(1000, TRUE);
	}
	return 1;
}

INT_PTR CALLBACK DialogProc(
	_In_  HWND hwndDlg,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	);



//////////////////////////////////////////////
HINSTANCE g_hIntance = NULL;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	g_hIntance = hInstance;
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, DialogProc);
	return 0;
}

INT_PTR CALLBACK DialogProc(
	_In_  HWND hwndDlg,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	)
{
	PrintWindowsMsg(hwndDlg, uMsg, wParam, lParam);
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
						  //反附加
						  AntiAttach();
 						  CheckSuspendCreateProcess();
 						  CheckBeforOEPThread();
 						 // AntiRemoteThread();
 						  MonitorLoadDll();
						  MonitorApc();
 						  CheckModPEInfo();
						  ImmMonitor();

						  TCHAR szAppInitDll[MAX_PATH] = { 0 };
						  GetAppInitDll(szAppInitDll, _countof(szAppInitDll));
						  SetDlgItemText(hwndDlg, IDC_EDIT_APPINIT_DLLS, szAppInitDll);
						  //
						  DWORD dwSelfThreadID = GetCurrentThreadId();
						  TCHAR szThreadIDBuff[MAX_PATH] = { 0 };
						  _stprintf_s(szThreadIDBuff, _countof(szThreadIDBuff), _T("%d"), dwSelfThreadID);
						  SetDlgItemText(hwndDlg, IDC_EDIT_THREAD_ID, szThreadIDBuff);
						  //
						  DWORD dwSelfProcessID = GetCurrentProcessId();
						  TCHAR szProcessIDBuff[MAX_PATH] = { 0 };
						  _stprintf_s(szProcessIDBuff, _countof(szProcessIDBuff), _T("%d"), dwSelfProcessID);
						  SetDlgItemText(hwndDlg, IDC_EDIT_PROCESS_ID, szProcessIDBuff);
						  //
						  DWORD dwApcThreadID = 0;
						  CreateThread(NULL, NULL, ApcThreadProc, NULL, NULL, &dwApcThreadID);
						  TCHAR szApcThreadIDBuff[MAX_PATH] = { 0 };
						  _stprintf_s(szApcThreadIDBuff, _countof(szApcThreadIDBuff), _T("%d"), dwApcThreadID);
						  SetDlgItemText(hwndDlg, IDC_EDIT_APC_THREAD_ID, szApcThreadIDBuff);
						  //弹窗
						  SetTimer(hwndDlg, LOOP_SEC, 1000, NULL);
						  
						  break;
	}
	case WM_TIMER:
	{
					 static bool bIsNotify1 = false;
					 static bool bIsNotify2 = false;
					 static bool bIsNotify3 = false;
					 static bool bIsNotify4 = false;
					 static bool bIsNotify5 = false;
					 static bool bIsNotify6 = false;
					 if (IsFondRemoteThread() && !bIsNotify1)
					 {
						 bIsNotify1 = true;
						 MessageBox(hwndDlg, _T("检测到远程线程注入"), _T("提示"), MB_OK);
					 }

					 if (IsFondShellCode() && !bIsNotify2)
					 {
						 bIsNotify2 = true;
						 MessageBox(hwndDlg, _T("检测到shellcode或者消息钩子注入"), _T("提示"), MB_OK);
					 }

					 if (IsFondApcInject() && !bIsNotify3)
					 {
						 bIsNotify3 = true;
						 MessageBox(hwndDlg, _T("检测到apc注入"), _T("提示"), MB_OK);
					 }

					 if (IsNormalCreateProcess() && !bIsNotify4)
					 {
						 bIsNotify4 = true;
						 MessageBox(hwndDlg, _T("以调试或者非正常方式启动"), _T("提示"), MB_OK);
					 }

					 if (IsFondModHidSelf() && !bIsNotify5)
					 {
						 bIsNotify4 = true;
						 MessageBox(hwndDlg, _T("检测到有隐藏自己的行为"), _T("提示"), MB_OK);
					 }
					 if (IsFondHackDll() && !bIsNotify6)
					 {
						 bIsNotify6 = true;
						 MessageBox(hwndDlg, _T("检测到dll劫持"), _T("提示"), MB_OK);
					 }
	}
		break;
	case WM_COMMAND:
	{
					   WORD wNotifyCode = HIWORD(wParam);
					   WORD wCtlID = LOWORD(wParam);
					   switch (wCtlID)
					   {
					   case IDC_BTN_THREAD:
					   {
										   CreateThread(NULL, NULL, TestThreadProc, NULL, NULL, NULL);
					   }
					   case IDC_BTN_LOAD_LIB:
					   {
												LoadLibrary(_T("F:\\15pb练习\\Windows平台\\Debug\\008dll劫持注入-被劫持的dll"));
					   }
						   break;
					   default:
						   break;
					   }
					   
	}
		break;
	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
		break;
	default:
		break;
	}
	return 0;
}
