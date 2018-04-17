
// 006菜单栏的添加.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CMy006菜单栏的添加App: 
// 有关此类的实现，请参阅 006菜单栏的添加.cpp
//

class CMy006菜单栏的添加App : public CWinApp
{
public:
	CMy006菜单栏的添加App();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
	afx_msg void On32773();
};

extern CMy006菜单栏的添加App theApp;