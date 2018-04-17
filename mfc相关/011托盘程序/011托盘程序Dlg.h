
// 011托盘程序Dlg.h : 头文件
//

#pragma once


// CMy011托盘程序Dlg 对话框
class CMy011托盘程序Dlg : public CDialogEx
{
// 构造
public:
	CMy011托盘程序Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MY011_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	NOTIFYICONDATA m_nid;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
protected:
	afx_msg LRESULT On托盘通知(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void On32771();
};
