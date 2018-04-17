
// 006工具栏的添加.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CMy006工具栏的添加App: 
// 有关此类的实现，请参阅 006工具栏的添加.cpp
//

class CMy006工具栏的添加App : public CWinApp
{
public:
	CMy006工具栏的添加App();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CMy006工具栏的添加App theApp;