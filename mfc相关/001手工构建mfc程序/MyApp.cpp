#include "MyApp.h"
#include "MyDialog.h"
#include "resource.h"

MyApp::MyApp()
{
}


MyApp::~MyApp()
{
}

BOOL MyApp::InitInstance()
{
	MyDialog dlg(IDD_DIALOG1);
	dlg.DoModal();

	return TRUE;
}
