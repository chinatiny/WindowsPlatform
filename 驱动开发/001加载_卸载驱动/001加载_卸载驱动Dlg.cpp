
// 001加载_卸载驱动Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "001加载_卸载驱动.h"
#include "001加载_卸载驱动Dlg.h"
#include "afxdialogex.h"
#include <Winsvc.h>
#include <shlwapi.h>  
#pragma comment(lib,"shlwapi.lib")  

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


// CMy001加载_卸载驱动Dlg 对话框



CMy001加载_卸载驱动Dlg::CMy001加载_卸载驱动Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMy001加载_卸载驱动Dlg::IDD, pParent)
	, m_strSysPath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy001加载_卸载驱动Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strSysPath);
}

BEGIN_MESSAGE_MAP(CMy001加载_卸载驱动Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMy001加载_卸载驱动Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMy001加载_卸载驱动Dlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMy001加载_卸载驱动Dlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CMy001加载_卸载驱动Dlg 消息处理程序

BOOL CMy001加载_卸载驱动Dlg::OnInitDialog()
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

void CMy001加载_卸载驱动Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMy001加载_卸载驱动Dlg::OnPaint()
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
HCURSOR CMy001加载_卸载驱动Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMy001加载_卸载驱动Dlg::OnBnClickedButton1()
{
	OPENFILENAME	openFile;
	TCHAR szFindFile[MAX_PATH] = { 0 };
	ZeroMemory(&openFile, sizeof(openFile));
	openFile.lStructSize = sizeof(openFile);
	openFile.lpstrFilter = _T("驱动文件(*.sys)\0*.sys\0所有文件(*.*)\0*.*\0\0");
	openFile.lpstrFile = szFindFile;
	openFile.nMaxFile = MAX_PATH;
	openFile.nFilterIndex = 1;
	openFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (!GetOpenFileName(&openFile))
	{
		DWORD dwErrCode = GetLastError();
		if (0 != dwErrCode)
		{
			CString strErrMsg;
			strErrMsg.Format(_T("打开文件失败，错误码:%d"), dwErrCode);
			AfxMessageBox(strErrMsg);
		}
		return;
	}
	m_strSysPath = szFindFile;
	UpdateData(FALSE);
}


void CMy001加载_卸载驱动Dlg::OnBnClickedButton2()
{

	SC_HANDLE hServiceMgr = NULL;//SCM管理器的句柄  
	SC_HANDLE hServiceDDK = NULL;//NT驱动程序的服务句柄  
	DWORD dwRtn = 0;
	BOOL bRet = 0;

	if (m_strSysPath.IsEmpty())
	{
		AfxMessageBox(_T("请先选择驱动程序"));
		goto __EXIT;
	}

	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == hServiceMgr)
	{
		AfxMessageBox(_T("打开驱动管理失败"));
		goto __EXIT;
	}

	TCHAR szDriverName[MAX_PATH] = { 0 };
	_tcscpy_s(szDriverName, _countof(szDriverName), m_strSysPath);
	PathStripPath(szDriverName);


	hServiceDDK = CreateService(hServiceMgr,
		szDriverName, //驱动程序的在注册表中的名字    
		szDriverName, // 注册表驱动程序的 DisplayName 值    
		SERVICE_ALL_ACCESS, // 加载驱动程序的访问权限    
		SERVICE_KERNEL_DRIVER,// 表示加载的服务是驱动程序    
		SERVICE_DEMAND_START, // 注册表驱动程序的 Start 值    
		SERVICE_ERROR_IGNORE, // 注册表驱动程序的 ErrorControl 值    
		m_strSysPath, // 注册表驱动程序的 ImagePath 值    
		NULL,  //GroupOrder HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\GroupOrderList  
		NULL,
		NULL,
		NULL,
		NULL);

	//判断服务是否失败  
	if (NULL == hServiceDDK)
	{
		dwRtn = GetLastError();
		if (dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_EXISTS)
		{
			//由于其他原因创建服务失败  
			AfxMessageBox(_T("CrateService() Faild"));
			goto __EXIT;
		}
		else
		{
			// 驱动程序已经加载，只需要打开    
			hServiceDDK = OpenService(hServiceMgr, szDriverName, SERVICE_ALL_ACCESS);
			if (hServiceDDK == NULL)
			{
				AfxMessageBox(_T("OpenService() Faild! \n"));
				goto __EXIT;
			}
		}
	}
	//开启此项服务  
	bRet = StartService(hServiceDDK, NULL, NULL);

	if(!bRet)
	{
		DWORD dwRtn = GetLastError();
		if (dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_ALREADY_RUNNING)
		{
			AfxMessageBox(_T("StartService() Faild! \n"));
			goto __EXIT;
		}
		else
		{
			if (dwRtn == ERROR_IO_PENDING)
			{
				//设备被挂住  
				AfxMessageBox(_T("StartService() Faild ERROR_IO_PENDING ! \n"));
				goto __EXIT;
			}
			else
			{
				//服务已经开启  
				AfxMessageBox(_T("StartService() Faild ERROR_SERVICE_ALREADY_RUNNING ! \n"));
				goto __EXIT;
			}
		}
	}
	else
	{
		AfxMessageBox(_T("StartService() Succ\n"));
	}

__EXIT:
	if (NULL != hServiceMgr) CloseServiceHandle(hServiceMgr);
	if (NULL != hServiceDDK) CloseServiceHandle(hServiceDDK);

}


void CMy001加载_卸载驱动Dlg::OnBnClickedButton3()
{
	SC_HANDLE hServiceMgr = NULL;//SCM管理器的句柄  
	SC_HANDLE hServiceDDK = NULL;//NT驱动程序的服务句柄  
	if (m_strSysPath.IsEmpty())
	{
		AfxMessageBox(_T("请先选择驱动程序"));
		goto __EXIT;
	}

	SERVICE_STATUS SvrSta;
	//打开SCM管理器  
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hServiceMgr == NULL)
	{
		//带开SCM管理器失败  
		AfxMessageBox(_T("OpenSCManager() Faild! \n"));
		goto __EXIT;
	}

	//打开驱动所对应的服务  
	TCHAR szDriverName[MAX_PATH] = { 0 };
	_tcscpy_s(szDriverName, _countof(szDriverName), m_strSysPath);
	PathStripPath(szDriverName);
	hServiceDDK = OpenService(hServiceMgr, szDriverName, SERVICE_ALL_ACCESS);
	if (NULL==hServiceDDK)
	{
		//打开驱动所对应的服务失败  
		AfxMessageBox(_T("OpenService() Faild! \n"));
		goto __EXIT;
	}

	if (!ControlService(hServiceDDK, SERVICE_CONTROL_STOP, &SvrSta))
	{
		AfxMessageBox(_T("ControlService() Faild!\n"));
		goto __EXIT;
	}

	//动态卸载驱动程序。    
	if (!DeleteService(hServiceDDK))
	{
		//卸载失败  
		AfxMessageBox(_T("DeleteSrevice() Faild!\n"));
	}
	else
	{
		AfxMessageBox(_T("DeleteService() Succ\n"));
	}


__EXIT:
	if (NULL != hServiceMgr) CloseServiceHandle(hServiceMgr);
	if (NULL != hServiceDDK) CloseServiceHandle(hServiceDDK);

}
