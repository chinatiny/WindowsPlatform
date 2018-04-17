#include <windows.h>
#include <tchar.h>
#include "Test.h"

void TestFunc()
{
	MessageBox(NULL, _T("被劫持的dll弹窗"), _T("提示"), MB_OK);
}