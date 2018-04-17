
// 006菜单栏的添加Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "006菜单栏的添加.h"
#include "006菜单栏的添加Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy006菜单栏的添加Dlg 对话框



CMy006菜单栏的添加Dlg::CMy006菜单栏的添加Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMy006菜单栏的添加Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy006菜单栏的添加Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMy006菜单栏的添加Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_32771, &CMy006菜单栏的添加Dlg::OnDoAction)
END_MESSAGE_MAP()


BOOL CMy006菜单栏的添加Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	////////////////////////////////////////////////////////////////////////
	/////////////////重要菜单的结构式十字链表////////////////////////////
	////////////////////////////////////////////////////////////////////////

	//1.1 主菜单直接加载资源(有个缺点必须创建一个子菜单)
 	m_mainMenu.LoadMenu(MAKEINTRESOURCE(IDR_MENU1));  // => 直接加载资源

	//1.2 主菜单动态加载(比较灵活)=>如果需要创建顶层菜单下面这行解开注释
	//m_mainMenu.CreateMenu(); // 创建顶层菜单.
	m_menuTest.CreatePopupMenu(); // 创建弹出菜单
	m_mainMenu.AppendMenu(MF_POPUP, (UINT_PTR)m_menuTest.m_hMenu, _T("动态创建测试"));


	//2. 动态创建菜单的子菜单
	CMenu animalMenu;              //定义菜单类对象  
	animalMenu.CreatePopupMenu();  //创建弹出菜单窗口  
	//m_mainMenu.AppendMenuW(MF_POPUP, (UINT)m_PopupMenu.m_hMenu, _T("动物"));  => //插入菜单和m_mainMenu同级
	m_mainMenu.GetSubMenu(1)->AppendMenu(MF_POPUP, (UINT)animalMenu.m_hMenu, _T("动物")); //插入菜单是m_mainMenu子菜单

	animalMenu.AppendMenu(MF_POPUP, 10001, _T("猫"));            //插入子菜单  
	animalMenu.AppendMenu(MF_POPUP, 10002, _T("狗"));            //插入子菜单  
	animalMenu.AppendMenu(MF_POPUP, 10003, _T("猴"));         //插入子菜单  
	animalMenu.AppendMenu(MF_POPUP, -1, _T("植物"));                  //插入兄弟菜单  
	animalMenu.Detach();                                               //分离菜单句柄  
	SetMenu(&m_mainMenu);                                                   //将菜单和窗口进行关







	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMy006菜单栏的添加Dlg::OnPaint()
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
HCURSOR CMy006菜单栏的添加Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMy006菜单栏的添加Dlg::OnDoAction()
{
	// TODO:  在此添加命令处理程序代码
	MessageBox(_T("受到点击事件"), _T("提示"), MB_OK);
}
