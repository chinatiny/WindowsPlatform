#include <windows.h>
#include <stdio.h>


int main(int argc, char* argv[])
{
	// 1. 创建邮槽对象
	HANDLE hSlot = CreateMailslot(
		L"\\\\.\\mailslot\\Sample",    // 邮槽名
		0,         // 不限制消息大小 
		MAILSLOT_WAIT_FOREVER,         // 无超时 
		(LPSECURITY_ATTRIBUTES)NULL);  // 安全属性
	// 2. 循环读取邮槽信息
	while (true) {
		// 2.1 获取邮槽消息数量
		DWORD dwMsgCount = 0, dwMsgSize = 0;
		GetMailslotInfo(     /* 获取邮槽信息 */
			hSlot,           // 邮槽句柄 
			(LPDWORD)NULL,// 无最大消息限制
			&dwMsgSize,      // 下一条消息的大小
			&dwMsgCount,     // 消息的数量
			(LPDWORD)NULL);// 无时限
		if (dwMsgSize == MAILSLOT_NO_MESSAGE) {
			Sleep(2000);
			continue;
		}
		// 2.2 循环获取全部消息（有可能不只一条）
		while (dwMsgCount) {
			PBYTE lpBuffer;
			lpBuffer = new BYTE[dwMsgSize + 0x10];
			// 读取邮槽中的信息
			DWORD dwRet;
			ZeroMemory(lpBuffer, dwMsgSize);
			if (!ReadFile(hSlot, lpBuffer, dwMsgSize, &dwRet, NULL)) {
				printf("ReadFile函数执行失败，错误码：%d.\n", GetLastError());
				delete[] lpBuffer;
				return 0;
			}
			// 显示信息
			printf("邮槽的内容: %s\n", lpBuffer);
			// 计算剩余的消息数
			GetMailslotInfo(hSlot, (LPDWORD)NULL, &dwMsgSize, &dwMsgCount, nullptr);
			delete[] lpBuffer;
		}
	}
	return 0;


	return 0;
}