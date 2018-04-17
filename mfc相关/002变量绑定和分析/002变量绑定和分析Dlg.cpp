
// 002变量绑定和分析Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "002变量绑定和分析.h"
#include "002变量绑定和分析Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy002变量绑定和分析Dlg 对话框



CMy002变量绑定和分析Dlg::CMy002变量绑定和分析Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMy002变量绑定和分析Dlg::IDD, pParent)
	, m_strEdit(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy002变量绑定和分析Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Text(pDX, IDC_EDIT1, m_strEdit);
}

BEGIN_MESSAGE_MAP(CMy002变量绑定和分析Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMy002变量绑定和分析Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMy002变量绑定和分析Dlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CMy002变量绑定和分析Dlg 消息处理程序

BOOL CMy002变量绑定和分析Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMy002变量绑定和分析Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMy002变量绑定和分析Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMy002变量绑定和分析Dlg::OnBnClickedButton1()
{
	CString strOld;
	m_edit.GetWindowText(strOld);
	CString strNew;
	strNew.Format(_T("%s\r\n测试变量绑定，来自 edit类型变量"), strOld.GetBuffer(0));
	m_edit.SetWindowText(strNew);
}


void CMy002变量绑定和分析Dlg::OnBnClickedButton2()
{
	UpdateData(TRUE);
	CString strOld = m_strEdit;
	m_strEdit.Format(_T("%s\r\n测试变量绑定，来自 Cstring类型变量"), strOld.GetBuffer(0));
	UpdateData(FALSE);
}
