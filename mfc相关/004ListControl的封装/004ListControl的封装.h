
// 004ListControl的封装.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CMy004ListControl的封装App: 
// 有关此类的实现，请参阅 004ListControl的封装.cpp
//

class CMy004ListControl的封装App : public CWinApp
{
public:
	CMy004ListControl的封装App();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CMy004ListControl的封装App theApp;