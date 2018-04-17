
// 007状态栏Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "007状态栏.h"
#include "007状态栏Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy007状态栏Dlg 对话框



CMy007状态栏Dlg::CMy007状态栏Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMy007状态栏Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy007状态栏Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMy007状态栏Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CMy007状态栏Dlg 消息处理程序

BOOL CMy007状态栏Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	// 添加状态栏
	// 使用的步骤:
	// 1. 创建一个状态栏 : Create
	// 2. 添加分栏(设置分栏的ID):SetIndicators( panelId , 2 );
	// 3. 设置分栏的信息:SetPaneInfo
	//  3.1 设置分栏的宽度
	//  3.2 设置分栏的显示文本

	// 1. 创建一个状态栏
	m_statusBar.Create(this);

	// 2. 在状态栏上创建显示文本的分栏
	UINT panelId[] = { WM_USER + 4, WM_USER + 5 };
	m_statusBar.SetIndicators(panelId, 2);

	m_statusBar.SetPaneInfo(0, WM_USER + 4, SBPS_NORMAL, 100);
	m_statusBar.SetPaneInfo(1, 0, SBPS_NORMAL, 500);
	m_statusBar.SetPaneText(0, L"状态栏文本1");
	m_statusBar.SetPaneText(1, L"状态栏文本2");

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);



	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMy007状态栏Dlg::OnPaint()
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
HCURSOR CMy007状态栏Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

