
// 002变量绑定和分析Dlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CMy002变量绑定和分析Dlg 对话框
class CMy002变量绑定和分析Dlg : public CDialogEx
{
// 构造
public:
	CMy002变量绑定和分析Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MY002_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// //IDC_EDIT1控件类型的变量	// //IDC_EDIT1控件类型的变量
	CEdit m_edit;
	CString m_strEdit;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
