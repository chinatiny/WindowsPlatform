// 延时导入dll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include <windows.h>
#include <tchar.h>
#include "DllInterface.h"


void TestFunc()
{
	MessageBox(NULL, _T("延时导入测试"), _T("测试"), MB_OK);
}

