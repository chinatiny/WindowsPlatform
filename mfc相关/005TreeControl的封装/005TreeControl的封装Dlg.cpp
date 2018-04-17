
// 005TreeControl的封装Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "005TreeControl的封装.h"
#include "005TreeControl的封装Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy005TreeControl的封装Dlg 对话框



CMy005TreeControl的封装Dlg::CMy005TreeControl的封装Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMy005TreeControl的封装Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy005TreeControl的封装Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_treeCtl);
}

BEGIN_MESSAGE_MAP(CMy005TreeControl的封装Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CMy005TreeControl的封装Dlg 消息处理程序

BOOL CMy005TreeControl的封装Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	m_imgList.Create(32, 32, ILC_COLOR32, 1, 1);
	m_imgList.Add(m_hIcon);


	//
	m_treeCtl.SetImageList(&m_imgList, TVSIL_NORMAL);

	// 1. 插入节点.
	m_treeCtl.InsertItem(_T("根节点1"), 0/*图标链表中的图标的序号*/);

	HTREEITEM hItem2 = m_treeCtl.InsertItem(L"根节点2", 0);
	m_treeCtl.InsertItem(_T("根节点3"), 0);

	m_treeCtl.InsertItem(_T("子节点1"), 0, 0,
		hItem2 /*新节点的父节点句柄*/);

	m_treeCtl.InsertItem(_T("子节点2"), 0, 0, hItem2);



	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMy005TreeControl的封装Dlg::OnPaint()
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
HCURSOR CMy005TreeControl的封装Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

