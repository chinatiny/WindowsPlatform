
// 006工具栏的添加Dlg.h : 头文件
//

#pragma once


// CMy006工具栏的添加Dlg 对话框
class CMy006工具栏的添加Dlg : public CDialogEx
{
// 构造
public:
	CMy006工具栏的添加Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MY006_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CToolBar	m_toolBar;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
