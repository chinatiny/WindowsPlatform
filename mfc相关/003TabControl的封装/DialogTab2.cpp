// DialogTab2.cpp : 实现文件
//

#include "stdafx.h"
#include "003TabControl的封装.h"
#include "DialogTab2.h"
#include "afxdialogex.h"


// DialogTab2 对话框

IMPLEMENT_DYNAMIC(DialogTab2, CDialog)

DialogTab2::DialogTab2(CWnd* pParent /*=NULL*/)
	: CDialog(DialogTab2::IDD, pParent)
{
	CreateDlg(MAKEINTRESOURCE(DialogTab2::IDD), pParent);
}

DialogTab2::~DialogTab2()
{
}

void DialogTab2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DialogTab2, CDialog)
END_MESSAGE_MAP()


// DialogTab2 消息处理程序
