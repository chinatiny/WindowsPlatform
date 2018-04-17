#pragma once


// DialogTab2 对话框

class DialogTab2 : public CDialog
{
	DECLARE_DYNAMIC(DialogTab2)

public:
	DialogTab2(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DialogTab2();

// 对话框数据
	enum { IDD = IDD_DIALOGTAB2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
