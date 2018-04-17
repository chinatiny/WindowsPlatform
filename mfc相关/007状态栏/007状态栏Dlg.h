
// 007状态栏Dlg.h : 头文件
//

#pragma once


// CMy007状态栏Dlg 对话框
class CMy007状态栏Dlg : public CDialogEx
{
// 构造
public:
	CMy007状态栏Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MY007_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CStatusBar	m_statusBar;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
