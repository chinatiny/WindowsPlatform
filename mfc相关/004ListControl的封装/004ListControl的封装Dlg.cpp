
// 004ListControl的封装Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "004ListControl的封装.h"
#include "004ListControl的封装Dlg.h"
#include "afxdialogex.h"
#include "Dbg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy004ListControl的封装Dlg 对话框



CMy004ListControl的封装Dlg::CMy004ListControl的封装Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMy004ListControl的封装Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy004ListControl的封装Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listCtl);
}

BEGIN_MESSAGE_MAP(CMy004ListControl的封装Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CMy004ListControl的封装Dlg::OnClickList1)
END_MESSAGE_MAP()


// CMy004ListControl的封装Dlg 消息处理程序

BOOL CMy004ListControl的封装Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	m_imageList.Create(32, 32, ILC_COLOR32 | ILC_MASK, 4, 1);
	for (int i = 0; i < 4; i++)
	{
		HICON hIcon1 = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON1 + i));
		m_imageList.Add(hIcon1);
	}

	//
	m_listCtl.SetGridStyle();
	m_listCtl.SetImageList(&m_imageList, LVSIL_SMALL);
	//
	m_listCtl.InsertColumn(0, _T("PID"), 100);
	m_listCtl.InsertColumn(1, _T("进程名"), 100);
	m_listCtl.InsertColumn(2, _T("路径"), 200);

	m_listCtl.SetItemText(0, 0, 3, _T("1001"), _T("QQ"), _T("c:\\qq.exe"));
	m_listCtl.SetItemText(1, 1, 3, _T("1002"), _T("迅雷"), _T("d:\\thunkder.exe"));
	srand((unsigned int )time(NULL));
	for (int i = 2; i < 100; i++)
	{
		int nRandomValue1 = rand();
		int nRandomValue2 = rand();
		int nRandomValue3 = rand();
		TCHAR szValue1Buff[MAX_PATH];
		TCHAR szValue2Buff[MAX_PATH];
		TCHAR szValue3Buff[MAX_PATH];

		_itot_s(nRandomValue1, szValue1Buff, _countof(szValue1Buff), 10);
		_itot_s(nRandomValue2, szValue2Buff, _countof(szValue2Buff), 10);
		_itot_s(nRandomValue3, szValue3Buff, _countof(szValue3Buff), 10);

		m_listCtl.SetItemText(i, i % 4, 3, szValue1Buff, szValue2Buff, szValue3Buff);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMy004ListControl的封装Dlg::OnPaint()
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
HCURSOR CMy004ListControl的封装Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMy004ListControl的封装Dlg::OnClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;
	PrintDbgInfo(_T("当前列表框有:%d行, %d列"), m_listCtl.GetRowNum(), m_listCtl.GetColNum());

	//
	std::vector<int > vecRow;
	m_listCtl.GetSelectedRow(vecRow);
	for (size_t i = 0; i < vecRow.size(); i++)
	{
		PrintDbgInfo(_T("当前选中的为%d行"), vecRow[i]);
	}
	//
	for (int i = 0; i < m_listCtl.GetColNum(); i++)
	{
		CString strText = m_listCtl.GetSpecItemText(vecRow[0], i);
		PrintDbgInfo(_T("%d行%d列的文本为:%s"), vecRow[0], i, strText);
	}




}
