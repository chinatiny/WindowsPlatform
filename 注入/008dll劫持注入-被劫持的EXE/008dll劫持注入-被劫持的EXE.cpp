// 008dll劫持注入-被劫持的EXE.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Test.h"
#include <windows.h>


int _tmain(int argc, _TCHAR* argv[])
{
	TestFunc();

	system("pause");
	return 0;
}

