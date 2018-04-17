#include <windows.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	// 1. 打开邮槽对象
	HANDLE hFile = CreateFile(
		L"\\\\.\\mailslot\\Sample", // 邮槽名称
		GENERIC_WRITE,      // 读写属性
		FILE_SHARE_READ,       // 共享属性
		NULL,                       // 安全属性
		OPEN_EXISTING,      // 打开方式
		FILE_ATTRIBUTE_NORMAL,      // 标志位
		NULL);                     // 文件模板（默认留空）
	// 2. 向mailslot写入
	DWORD  dwWritten;
	LPSTR  lpMessage = "邮槽测试消息！";
	DWORD  dwMegLen = strlen(lpMessage) + sizeof(CHAR);
	WriteFile(hFile, lpMessage, dwMegLen, &dwWritten, NULL);
	// 3. 结束
	printf("已经向邮槽写入信息！\n");
	CloseHandle(hFile);
}