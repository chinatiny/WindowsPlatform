
// 013异形窗口Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "013异形窗口.h"
#include "013异形窗口Dlg.h"
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


// CMy013异形窗口Dlg 对话框



CMy013异形窗口Dlg::CMy013异形窗口Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMy013异形窗口Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy013异形窗口Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMy013异形窗口Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_NCHITTEST()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


// CMy013异形窗口Dlg 消息处理程序

BOOL CMy013异形窗口Dlg::OnInitDialog()
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
	// 1. 添加扩展风格
	DWORD dwStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
	SetWindowLong(m_hWnd, GWL_EXSTYLE, dwStyle | WS_EX_LAYERED);

	// 2. 设置透明色(设为黑色)
	SetLayeredWindowAttributes(RGB(0, 0, 0), 0, LWA_COLORKEY);

	// 3. 在WM_PAIN中,把预先画好的图片绘制到窗口
	m_bgimg.LoadFromResource(AfxGetInstanceHandle(),
		IDB_BITMAP1);

	//4.  在wm_paint将图片拉伸拷贝到目标DC中(客户区DC)
	
	//5. OnNcHitTest,需要处理下WM_NCHITTEST消息，不然没法拖动窗体



	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMy013异形窗口Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMy013异形窗口Dlg::OnPaint()
{
	CClientDC dc(this);
	CRect rt;
	GetClientRect(rt);
	if (IsIconic())
	{
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
		// 4. 将图片拉伸拷贝到目标DC中(客户区DC)
		m_bgimg.StretchBlt(dc.m_hDC, rt, SRCCOPY);
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMy013异形窗口Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



LRESULT CMy013异形窗口Dlg::OnNcHitTest(CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	ScreenToClient(&point);
	if (point.y >= 0 && point.y <= 25)
	{
		return HTCAPTION;
	}
	else
	{
		return HTCLIENT;
	}

}


void CMy013异形窗口Dlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CMenu menu;
	menu.LoadMenu(IDR_MENU1);
	CMenu *pSubMenu = menu.GetSubMenu(0);

	POINT cusorPoint;
	GetCursorPos(&cusorPoint);
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, cusorPoint.x, cusorPoint.y, this);

	CDialogEx::OnRButtonDown(nFlags, point);
}
