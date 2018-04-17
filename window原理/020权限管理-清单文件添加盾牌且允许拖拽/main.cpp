#include "resource.h"
#include <shlobj.h>
#include <tchar.h>

BOOL    IsAdmin(HANDLE hProcess);

INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
}

INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch (uMsg) {

	case WM_INITDIALOG:
	{
						  //添加这个，就算加了uac盾牌
						  ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
						  ChangeWindowMessageFilter(0x0049, MSGFLT_ADD);
	}
		break;
	case WM_DROPFILES:
	{
						 HDROP hDrop = (HDROP)((VOID*)wParam);
						 int nCntFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
						 //
						 TCHAR szDrayFileName[MAX_PATH];
						 for (int i = 0; i < nCntFiles; i++)
						 {
							 DragQueryFile(hDrop, i, szDrayFileName, _countof(szDrayFileName));
							 MessageBox(hWnd, szDrayFileName, _T("拽文件"), MB_OK);
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


