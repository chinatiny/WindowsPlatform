#include <windows.h>
#include <winternl.h>
#pragma  comment (lib,"ntdll.lib")
bool NQIP_ProcessDebugFlag()
{
	BOOL bProcessDebugFlag = 0;
	NtQueryInformationProcess(GetCurrentProcess(), 		// 目标进程句柄
		(PROCESSINFOCLASS)0x1F, 	// 查询信息类型
		&bProcessDebugFlag, 		// 输出查询信息
		sizeof(bProcessDebugFlag), 	// 查询类型大小
		NULL 				// 实际返回大小
		);

	return bProcessDebugFlag ? false : true;
}


int main(int argc, char* argv[])
{
	bool bIsDebug = NQIP_ProcessDebugFlag();
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

