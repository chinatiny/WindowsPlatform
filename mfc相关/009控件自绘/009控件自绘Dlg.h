
// 009控件自绘Dlg.h : 头文件
//

#pragma once
#include "MyButtonWM_PAINT.h"
#include "MyDrawItemButton.h"


// CMy009控件自绘Dlg 对话框
class CMy009控件自绘Dlg : public CDialogEx
{
// 构造
public:
	CMy009控件自绘Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MY009_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	MyButtonWM_PAINT m_WM_PAINT_Button;
	CMyDrawItemButton m_drawItemButton;
//	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
};
