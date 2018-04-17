
// 007三环和零环通信_ring3Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "007三环和零环通信_ring3.h"
#include "007三环和零环通信_ring3Dlg.h"
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


// CMy007三环和零环通信_ring3Dlg 对话框



CMy007三环和零环通信_ring3Dlg::CMy007三环和零环通信_ring3Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMy007三环和零环通信_ring3Dlg::IDD, pParent),
	m_hDevice(NULL)
	, m_strThreadID(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy007三环和零环通信_ring3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strThreadID);
}

BEGIN_MESSAGE_MAP(CMy007三环和零环通信_ring3Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMy007三环和零环通信_ring3Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMy007三环和零环通信_ring3Dlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMy007三环和零环通信_ring3Dlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CMy007三环和零环通信_ring3Dlg 消息处理程序

BOOL CMy007三环和零环通信_ring3Dlg::OnInitDialog()
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
	DWORD dwThreadID =  GetCurrentThreadId();
	m_strThreadID.Format(_T("%d"), dwThreadID);
	UpdateData(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMy007三环和零环通信_ring3Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMy007三环和零环通信_ring3Dlg::OnPaint()
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
HCURSOR CMy007三环和零环通信_ring3Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


#define LINK_NAME L"\\\\.\\15PBHello"

void CMy007三环和零环通信_ring3Dlg::OnBnClickedButton1()
{
	m_hDevice = CreateFileW(
		LINK_NAME,
		GENERIC_ALL,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (NULL == m_hDevice || INVALID_HANDLE_VALUE == m_hDevice)
	{
		AfxMessageBox(_T("无法打开设备"));
	}
}


void CMy007三环和零环通信_ring3Dlg::OnBnClickedButton2()
{
	if (NULL != m_hDevice)
	{
		DWORD dwWrite;
		char szMsg[] = "hello 15pb";
		WriteFile(m_hDevice, szMsg, strlen(szMsg) + 1, &dwWrite, NULL);
		//
		CString strFormat;
		strFormat.Format(_T("real write:%d"), dwWrite);
		AfxMessageBox(strFormat);

	}
}


void CMy007三环和零环通信_ring3Dlg::OnBnClickedButton3()
{
	if (NULL != m_hDevice)
	{
		DWORD dwRealRead;
		wchar_t buff[MAX_PATH] = { 0 };
		ReadFile(m_hDevice, buff, sizeof(buff), &dwRealRead, NULL);
		CString strFormat;
		strFormat.Format(_T("收到数据:%s,, realread:%d"), buff, dwRealRead);
		AfxMessageBox(strFormat);
	}
}
