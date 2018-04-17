
// 010加载Dll实现画图板EXEDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "010加载Dll实现画图板EXE.h"
#include "010加载Dll实现画图板EXEDlg.h"
#include "afxdialogex.h"
#include "Dbg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy010加载Dll实现画图板EXEDlg 对话框



CMy010加载Dll实现画图板EXEDlg::CMy010加载Dll实现画图板EXEDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMy010加载Dll实现画图板EXEDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy010加载Dll实现画图板EXEDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMy010加载Dll实现画图板EXEDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void GetExeRootPath(TCHAR* pszDirPath, int nccLen)
{
	TCHAR szBuff[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, szBuff, sizeof(szBuff)-sizeof(TCHAR));
	::PathRemoveFileSpec(szBuff);
	_stprintf_s(pszDirPath, nccLen - 1, _T("%s%s"), szBuff, "\\");
}


BOOL EnumPlugin(void *Parameter, FileNode *pNode)
{
	CMy010加载Dll实现画图板EXEDlg *pDlg = (CMy010加载Dll实现画图板EXEDlg*)Parameter;
	pDlg->DoPluginFile(pNode);
	return FALSE;
}

void  CMy010加载Dll实现画图板EXEDlg::DoPluginFile(FileNode *pNode)
{
	HMODULE hDll = NULL;
	if (pNode->bIsDir) goto __NORMAL_END;
	// 加载DLL文件
	hDll = LoadLibrary(pNode->szFileName);
	if (NULL == hDll) goto __ERROR_END;
	// 获取导出函数
	typedef const TCHAR*(*GETNAME)();
	GETNAME pfnGetName = (GETNAME)GetProcAddress(hDll, "GetName");
	if (NULL == pfnGetName) goto __ERROR_END;
	//执行导出函数获取名字
	const TCHAR* pszDllName = pfnGetName();
	// 把DLL名插入到菜单.
	m_pluginMenu.AppendMenu(MF_STRING, m_nPluginID, pszDllName);
	//插入到kv映射中
	m_mapPluginDll.insert({ m_nPluginID, hDll });
	++m_nPluginID;
	goto __NORMAL_END;
__ERROR_END:
	if (NULL != hDll) FreeLibrary(hDll);

__NORMAL_END:
	;
}


// CMy010加载Dll实现画图板EXEDlg 消息处理程序

BOOL CMy010加载Dll实现画图板EXEDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	m_mainMenu.CreateMenu(); // 创建顶层菜单.
	m_pluginMenu.CreatePopupMenu(); // 创建弹出菜单

	m_nPluginID = WM_USER + 100;
	//获取插件目录
	TCHAR szPluginDir[MAX_PATH];
	GetExeRootPath(szPluginDir, _countof(szPluginDir));
	_stprintf_s(szPluginDir, _countof(szPluginDir) - _tcslen(szPluginDir), _T("%s%s"), szPluginDir, _T("plugin\\"));
	//
	FileOperation fOperation;
	FileNode *pRootNode = NULL;
	fOperation.EnumFile(szPluginDir, pRootNode, EnumPlugin, this, 0, _T("*.dll"));
	fOperation.DeleteFileNodeTree(pRootNode);
	//
	m_mainMenu.AppendMenu(MF_POPUP, (UINT_PTR)m_pluginMenu.m_hMenu, _T("插件"));
	// 设置对话框的菜单栏
	SetMenu(&m_mainMenu);
	


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMy010加载Dll实现画图板EXEDlg::OnPaint()
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
		if (m_pShap) m_pShap->DrawOld(GetDC());
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMy010加载Dll实现画图板EXEDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CMy010加载Dll实现画图板EXEDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD  wCtlID = LOWORD(wParam);
	auto ite = m_mapPluginDll.find(wCtlID);
	if (ite != m_mapPluginDll.end())
	{
		typedef  CShape* (*fpTypeGetInstance)();
		fpTypeGetInstance fpGetInstance = (fpTypeGetInstance)GetProcAddress(m_mapPluginDll[wCtlID], "GetInstance");
		m_pShap = fpGetInstance();
	}
	return CDialogEx::OnCommand(wParam, lParam);
}


void CMy010加载Dll实现画图板EXEDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_pShap) m_pShap->LButtonDown(point.x, point.y);
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CMy010加载Dll实现画图板EXEDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_pShap) m_pShap->LButtonUp(point.x, point.y);
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CMy010加载Dll实现画图板EXEDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_pShap && nFlags & MK_LBUTTON)
	{
		m_pShap->MouseMove(this, GetDC(), point.x, point.y);
	}

	CDialogEx::OnMouseMove(nFlags, point);
}
