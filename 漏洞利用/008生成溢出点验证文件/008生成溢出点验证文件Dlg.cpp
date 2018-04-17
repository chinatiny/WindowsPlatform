
// 008生成溢出点验证文件Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "008生成溢出点验证文件.h"
#include "008生成溢出点验证文件Dlg.h"
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


// CMy008生成溢出点验证文件Dlg 对话框



CMy008生成溢出点验证文件Dlg::CMy008生成溢出点验证文件Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMy008生成溢出点验证文件Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy008生成溢出点验证文件Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMy008生成溢出点验证文件Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMy008生成溢出点验证文件Dlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CMy008生成溢出点验证文件Dlg 消息处理程序

BOOL CMy008生成溢出点验证文件Dlg::OnInitDialog()
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

void CMy008生成溢出点验证文件Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMy008生成溢出点验证文件Dlg::OnPaint()
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
HCURSOR CMy008生成溢出点验证文件Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMy008生成溢出点验证文件Dlg::OnBnClickedButton1()
{
	OPENFILENAMEA ofn;       // 公共对话框结构。
	char szFile[MAX_PATH]; // 保存获取文件名称的缓冲区。          
	// 初始化选择文件对话框。
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = this->m_hWnd;
	ofn.lpstrFile = szFile;

	//
	ofn.lpstrFile[0] = 0;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All/0*.*/0Text/0*.TXT/0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
	//
	if (GetSaveFileNameA(&ofn))
	{
		FILE *fp = NULL;
		fopen_s(&fp, szFile, "w+");
		if (NULL == fp)
		{
			AfxMessageBox(_T("创建文件失败"));
			return;
		}
		else
		{
			//1. 数字和大写字符
			for (char i = '0'; i <= '9'; i++)
			{
				for (char j = 'A'; j < 'Z'; j++)
				{
					fprintf_s(fp, "%c%c", i, j);
				}
			}

			//2. 数字和小写字符
			for (char i = '0'; i <= '9'; i++)
			{
				for (char j = 'a'; j < 'z'; j++)
				{
					fprintf_s(fp, "%c%c", i, j);
				}
			}

			//3. 大写字符和数字
			for (char i = 'A'; i <= 'Z'; i++)
			{
				for (char j = '0'; j < '9'; j++)
				{
					fprintf_s(fp, "%c%c", i, j);
				}
			}

			//4 大写字符和小写字符
			for (char i = 'A'; i <= 'Z'; i++)
			{
				for (char j = 'a'; j < 'z'; j++)
				{
					fprintf_s(fp, "%c%c", i, j);
				}
			}

			//5. 小写字符和数字
			for (char i = 'a'; i <= 'z'; i++)
			{
				for (char j = '0'; j < '9'; j++)
				{
					fprintf_s(fp, "%c%c", i, j);
				}
			}


			//6.小写字符和大写字符
			for (char i = 'a'; i <= 'z'; i++)
			{
				for (char j = 'A'; j < 'Z'; j++)
				{
					fprintf_s(fp, "%c%c", i, j);
				}
			}

			//7. 数字和数字
			for (char i = '0'; i <= '9'; i++)
			{
				for (char j = '0'; j < '9'; j++)
				{
					fprintf_s(fp, "%c%c", i, j);
				}
			}

			//8.大写字符和大写字符
			for (char i = 'A'; i <= 'Z'; i++)
			{
				for (char j = 'A'; j < 'Z'; j++)
				{
					fprintf_s(fp, "%c%c", i, j);
				}
			}

			//9. 小写字符和小写字符
			for (char i = 'a'; i <= 'z'; i++)
			{
				for (char j = 'a'; j < 'z'; j++)
				{
					fprintf_s(fp, "%c%c", i, j);
				}
			}


			fclose(fp);
		}
	}
	else
	{
		AfxMessageBox(_T("请选择一个文件"));
		return;
	}
}
