
// 007三环和零环通信_ring3Dlg.h : 头文件
//

#pragma once


// CMy007三环和零环通信_ring3Dlg 对话框
class CMy007三环和零环通信_ring3Dlg : public CDialogEx
{
// 构造
public:
	CMy007三环和零环通信_ring3Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MY007_RING3_DIALOG };

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
	afx_msg void OnBnClickedButton1();

private:
	HANDLE m_hDevice;
public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	CString m_strThreadID;
};
