#include <windows.h>
#include <tchar.h>
#include <Windowsx.h>
#include "resource.h"
#include "Dbg.h"
#include <CommCtrl.h>

#pragma comment(lib,"Comctl32.lib ")

INT_PTR CALLBACK DialogProc(
	_In_  HWND hwndDlg,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	);

void InitImageList();
int  OnInitDialog(
	_In_  HWND hwndDlg,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	);


HINSTANCE g_hInstance = NULL;
HIMAGELIST g_hImageList = NULL;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	g_hInstance = hInstance;
	HWND hDlg = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc, NULL);
	ShowWindow(hDlg, SW_SHOW);

	HACCEL hAccTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	MSG msg = { 0 };
	while (GetMessage(&msg, 0, 0, 0))
	{
		if (!TranslateAccelerator(hDlg, hAccTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

INT_PTR CALLBACK DialogProc(
	_In_  HWND hwndDlg,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
						  InitCommonControls();
						  InitImageList();
						  OnInitDialog(hwndDlg, uMsg, wParam, lParam);
						  return 1;
	}
		break;
	case WM_COMMAND:
	{
					   WORD wHigh = HIWORD(wParam);
					   WORD wLow = LOWORD(wParam);
					   return 1;
	}
		break;
	case WM_CLOSE:
	{
					 PrintDbgInfo(_T("WM_CLOSE 消息被触发"));
					 PostQuitMessage(0);
					 return 1;
	}
		break;
	case WM_NOTIFY:// 复杂控件的通知消息
	{
					   NMHDR* pNmhdr = (NMHDR*)lParam;
					   /*List控件点击消息*/
					   if (pNmhdr->idFrom == IDC_LIST1 &&
						   pNmhdr->code == NM_CLICK)
					   {
						   LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;
						   TCHAR szList[MAX_PATH] = {};
						   ListView_GetItemText(
							   pNmhdr->hwndFrom,// LIST 控件句柄
							   lpnmitem->iItem, // 行号
							   lpnmitem->iSubItem,//列标
							   szList, // 获取到的文本保存位置
							   _countof(szList));
						   MessageBox(hwndDlg, szList, 0, MB_OK);
					   }
	}
	default:
		break;
	}
	return 0;
}

int  OnInitDialog(
	_In_  HWND hwndDlg,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	)
{
	/*List的初始化通过属性窗口或代码*/
	HWND hListCtrl = GetDlgItem(hwndDlg, IDC_LIST1);
	/*设置控件风格样式->所有控件*/
	// 原来的风格
	LRESULT lStyle = ListView_GetExtendedListViewStyle(hListCtrl);
	// 添加新的扩展风格 整行选中 带网格
	ListView_SetExtendedListViewStyle(hListCtrl, lStyle | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	/*给列表框关联图标数组*/
	ListView_SetImageList(hListCtrl, g_hImageList, LVSIL_SMALL);
	/*添加行列信息*/
	/*
	insertcolumn 插入列
	insertitem 插入X行第1列
	setitem 设置X行Y列的信息
	*/
	DWORD dwWidth[] = { 80, 120, 200 };
	TCHAR *szCol[] = { TEXT("姓名"), TEXT("阶段"), TEXT("状态") };
	for (int i = 0; i < 3; ++i)
	{
		LVCOLUMN lvc = {};
		lvc.cx = dwWidth[i];// 列宽
		lvc.pszText = szCol[i];
		lvc.mask = LVCF_TEXT | LVCF_WIDTH;
		ListView_InsertColumn(
			hListCtrl,
			i,// 列号
			&lvc);
	}
	/*添加行信息*/
	for (size_t i = 0; i < 20; i++)
	{
		LVITEM lvi = {};
		lvi.pszText = TEXT("赵钱孙李");
		lvi.iItem = i;// 第几行
		/*添加图标*/
		lvi.iImage = i % 4;// 插入的图标在图标数组中的索引，不能越界
		lvi.mask = LVIF_TEXT | LVIF_IMAGE;
		ListView_InsertItem(hListCtrl, &lvi);
		//lvi.iItem = i;// 第几行
		lvi.iSubItem = 1;// 第几列
		lvi.pszText = TEXT("第二阶段");
		ListView_SetItem(hListCtrl, &lvi);
		// 如果只是单纯的设置文本
		ListView_SetItemText(hListCtrl, i, 2, TEXT("挣扎"));
	}

	return 0;
}


void InitImageList()
{
	// 先创建数组
	g_hImageList = ImageList_Create(32, 32, ILC_COLOR32 | ILC_MASK, 4, 1);
	// 添加成员
	HINSTANCE hInstance = GetModuleHandle(0);
	for (int i = 0; i < 4; ++i)
	{
		HICON hIcon1 = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1 + i));
		ImageList_AddIcon(g_hImageList, hIcon1);
	}
}
