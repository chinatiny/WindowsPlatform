// 010加载Dll实现画图板_画直线.h : 010加载Dll实现画图板_画直线 DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CMy010加载Dll实现画图板_画直线App
// 有关此类实现的信息，请参阅 010加载Dll实现画图板_画直线.cpp
//

class CMy010加载Dll实现画图板_画直线App : public CWinApp
{
public:
	CMy010加载Dll实现画图板_画直线App();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
