#pragma once


// DialogTab1 对话框

class DialogTab1 : public CDialog
{
	DECLARE_DYNAMIC(DialogTab1)

public:
	DialogTab1(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DialogTab1();

// 对话框数据
	enum { IDD = IDD_DIALOGTAB1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};
