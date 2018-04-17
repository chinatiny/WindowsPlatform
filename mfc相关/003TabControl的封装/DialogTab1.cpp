// DialogTab1.cpp : 实现文件
//

#include "stdafx.h"
#include "003TabControl的封装.h"
#include "DialogTab1.h"
#include "afxdialogex.h"


// DialogTab1 对话框

IMPLEMENT_DYNAMIC(DialogTab1, CDialog)

DialogTab1::DialogTab1(CWnd* pParent /*=NULL*/)
	: CDialog(DialogTab1::IDD, pParent)
{
	CreateDlg(MAKEINTRESOURCE(DialogTab1::IDD), pParent);
}

DialogTab1::~DialogTab1()
{
}

void DialogTab1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DialogTab1, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &DialogTab1::OnBnClickedButton1)
END_MESSAGE_MAP()


// DialogTab1 消息处理程序


void DialogTab1::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
}
