// 001关于调试.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Dbg.h"


int _tmain(int argc, _TCHAR* argv[])
{
	CreateDbgConsole();
	for (int i = 0; i < 100; i++)
	{
		PrintDbgInfo(_T("名字:%s,  姓名:%d"), _T("hzh01"), i);
	}
	return 0;

}

