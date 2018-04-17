#include <windows.h>
#include <stdio.h>
#include "MsgHookInterface.h"

int main(int argc, char* argv[])
{
	DWORD dwThreadID = 0;
	printf("请输入需要hook的线程id:");
	scanf_s("%d", &dwThreadID);
	g_tid = dwThreadID;
	InstallHook();
	system("pause");
	UninstallHook();
	system("pause");

	return 0;
}