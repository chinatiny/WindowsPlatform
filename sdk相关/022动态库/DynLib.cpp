#include "DynLib.h"
#include <windows.h>
#include <tchar.h>

void TestDynLib()
{
	MessageBox(NULL, _T("这是一个静态库"), _T("提示"), MB_OK);
}
