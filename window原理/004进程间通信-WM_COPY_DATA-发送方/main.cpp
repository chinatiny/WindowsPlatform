#include <stdio.h>
#include <windows.h>
#include <tchar.h>

int main(int argc, char* argv[])
{
	HWND hTargetWindow = FindWindow(NULL, _T("copy data 测试"));

	if (NULL == hTargetWindow)
	{
		printf("没有找到目标窗体句柄\n");
		return 1;
	}

	COPYDATASTRUCT copyDta;
	//TCHAR szText[MAX_PATH] = _T("这是一个测试消息");
	TCHAR *szText = new TCHAR[1024 * 1024 * 3];
	memset(szText, 0, 1024 * 1024 *3);
	_tcscpy_s(szText, 1024 * 1024 * 3, _T("这是一个消息"));

	copyDta.lpData = szText;
	copyDta.cbData = 1024 * 1024 * 3 * sizeof(TCHAR);
	copyDta.dwData = sizeof(COPYDATASTRUCT);

	SendMessage(hTargetWindow, WM_COPYDATA, 0, (LPARAM)&copyDta);

	system("pause");
	return 0;
}