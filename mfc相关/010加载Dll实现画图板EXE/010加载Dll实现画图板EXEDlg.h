
// 010加载Dll实现画图板EXEDlg.h : 头文件
//

#pragma once

#include "FileOperation.h"
#include "DllInterface.h"
#include <map>


// CMy010加载Dll实现画图板EXEDlg 对话框
class CMy010加载Dll实现画图板EXEDlg : public CDialogEx
{
// 构造
public:
	CMy010加载Dll实现画图板EXEDlg(CWnd* pParent = NULL);	// 标准构造函数
	void DoPluginFile(FileNode *pNode);

// 对话框数据
	enum { IDD = IDD_MY010DLLEXE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	// 实现
protected:
	HICON m_hIcon;
	CMenu m_mainMenu;
	CMenu m_pluginMenu;
	int m_nPluginID;
	CShape *m_pShap;
	std::map<int, HMODULE> m_mapPluginDll;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
