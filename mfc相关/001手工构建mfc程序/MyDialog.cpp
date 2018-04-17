#include "MyDialog.h"
#include "resource.h"




MyDialog::MyDialog(UINT uID)
:CDialog(uID)
{

}

MyDialog::~MyDialog()
{
}

BOOL MyDialog::OnInitDialog()
{

	return CDialog::OnInitDialog();
}

void MyDialog::OnBnClickedButton()
{
	MessageBox(_T("响应按钮事件"), _T("提示"), MB_OK);
}

BEGIN_MESSAGE_MAP(MyDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON, &MyDialog::OnBnClickedButton)
END_MESSAGE_MAP()
