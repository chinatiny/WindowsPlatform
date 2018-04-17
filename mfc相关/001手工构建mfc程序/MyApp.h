#pragma once
#include "afxwin.h"
class MyApp :
	public CWinApp
{
public:
	MyApp();
	~MyApp();

	BOOL InitInstance() override;
};



MyApp theApp;

