#include <windows.h>
#include <winternl.h>
#include <stdio.h>

#pragma comment(lib,"ntdll.lib")
bool NQIP_ProcessDebugPort() {

	printf("开始检测debug port\n");
	HANDLE nDebugPort = 0;
	NtQueryInformationProcess(GetCurrentProcess(), // 目标进程句柄
		ProcessDebugPort, 	 // 查询信息类型
		&nDebugPort, 		 // 输出查询信息
		sizeof(nDebugPort),  // 查询类型大小
		NULL 			     // 实际返回数据大小
		);

	return nDebugPort == INVALID_HANDLE_VALUE ? true : false;
}


int main(int argc, char* argv[])
{

	bool bIsDebug = NQIP_ProcessDebugPort();
	if (bIsDebug)
	{
		MessageBoxA(NULL, "正在被调试", "警告", 0);
	}
	else
	{
		MessageBoxA(NULL, "现在很安全", "恭喜", 0);
	}
	return 0;
}

