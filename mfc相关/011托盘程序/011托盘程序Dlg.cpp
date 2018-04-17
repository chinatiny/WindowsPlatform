
// 011托盘程序Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "011托盘程序.h"
#include "011托盘程序Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void On32771();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMy011托盘程序Dlg 对话框



CMy011托盘程序Dlg::CMy011托盘程序Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMy011托盘程序Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy011托盘程序Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMy011托盘程序Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMy011托盘程序Dlg::OnBnClickedButton1)
	ON_MESSAGE(WM_USER + 1, &CMy011托盘程序Dlg::On托盘通知)
	ON_COMMAND(ID_32771, &CMy011托盘程序Dlg::On32771)
END_MESSAGE_MAP()


// CMy011托盘程序Dlg 消息处理程序

BOOL CMy011托盘程序Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMy011托盘程序Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMy011托盘程序Dlg::OnPaint()
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
HCURSOR CMy011托盘程序Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMy011托盘程序Dlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	memset(&m_nid, 0, sizeof(m_nid));
	m_nid.cbSize = sizeof(NOTIFYICONDATA);
	m_nid.hIcon = m_hIcon;
	m_nid.hWnd = m_hWnd;// 接收托盘消息的窗口句柄

	// 任务栏在检测我们的托盘图标有右键消息的时候,
	// 就会通过SendMessage发送注册的消息给我们
	// 的窗口
	// 如果要响应该消息, 可以通过类向导的添加自定义
	// 消息来响应.
	m_nid.uCallbackMessage = WM_USER + 1;
	_tcscpy_s(m_nid.szTip, _countof(m_nid.szTip), _T("托盘小窗口"));
	m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	// 添加托盘图标
	Shell_NotifyIcon(NIM_ADD, &m_nid);

	//删除托盘
	//Shell_NotifyIcon(NIM_DELETE, &nid);
	//
	ShowWindow(SW_HIDE);


}



afx_msg LRESULT CMy011托盘程序Dlg::On托盘通知(WPARAM wParam, LPARAM lParam)
{
	/*
	wParam接收的是图标的ID，而lParam接收的是鼠标的行为 
	鼠标左键可以控制窗口的显示与隐藏
	鼠标右键可以控制弹出菜单   
	*/
	static BOOL show = FALSE;
	DWORD dwCode = lParam;
	if (lParam == WM_LBUTTONDOWN) {
		// 鼠标左键单击
		ShowWindow(show ? SW_SHOW : SW_HIDE);
		Shell_NotifyIcon(NIM_DELETE, &m_nid);
		show = ~show;
	}
	else if (lParam == WM_RBUTTONDOWN) {
		// 鼠标右键单击

		// 弹出一个右键菜单.
		CMenu menu;
		menu.LoadMenu(IDR_MENU1);
		CMenu* subMenu = menu.GetSubMenu(0);
		CPoint pt;
		GetCursorPos(&pt);
		// 
		subMenu->TrackPopupMenu(TPM_LEFTALIGN,
			pt.x, pt.y, this);

	}

	return 0;
}



void CMy011托盘程序Dlg::On32771()
{
	// TODO:  在此添加命令处理程序代码
	Shell_NotifyIcon(NIM_DELETE, &m_nid);
	PostQuitMessage(0);
}
