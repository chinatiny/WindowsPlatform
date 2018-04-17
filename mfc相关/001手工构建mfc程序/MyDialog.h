#pragma once
#include "afxwin.h"

class MyDialog :
	public CDialog
{
public:
	MyDialog(UINT uID);
	~MyDialog();
	BOOL OnInitDialog() override;
	void OnBnClickedButton();


	DECLARE_MESSAGE_MAP()
};

