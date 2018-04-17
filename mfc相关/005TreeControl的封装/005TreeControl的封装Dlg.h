
// 005TreeControl的封装Dlg.h : 头文件
//

#pragma once
#include "afxcmn.h"


// CMy005TreeControl的封装Dlg 对话框
class CMy005TreeControl的封装Dlg : public CDialogEx
{
// 构造
public:
	CMy005TreeControl的封装Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MY005TREECONTROL_DIALOG };

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
	CTreeCtrl m_treeCtl;
	CImageList m_imgList;
};
