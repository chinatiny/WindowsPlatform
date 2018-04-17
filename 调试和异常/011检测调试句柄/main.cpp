#include <windows.h>
#include <winternl.h>
#pragma  comment (lib,"ntdll.lib")

bool NQIP_ProcessDebugObjectHandle()
{
	HANDLE hProcessDebugObjectHandle = 0;
	DWORD dwReturnLength = 0;
	NtQueryInformationProcess(GetCurrentProcess(), 			// 目标进程句柄
		(PROCESSINFOCLASS)0x1E, 		// 查询信息类型
		&hProcessDebugObjectHandle, 	// 输出查询信息
		sizeof(hProcessDebugObjectHandle), // 查询类型大小
		&dwReturnLength 					// 实际返回大小
		);
	MessageBoxA(NULL, "检测弹窗，方便调试", "警告", 0);
	return hProcessDebugObjectHandle ? true : false;
}

int main(int argc, char* argv[])
{

	bool bIsDebug = NQIP_ProcessDebugObjectHandle();

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
