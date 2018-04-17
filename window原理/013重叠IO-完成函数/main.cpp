#include "MyOVERLAPPED.h"
#include <tchar.h>
#include <stdio.h>

//   以下几个函数可以设置线程为可警醒的状态：
//   1. SleepEx
//   2. WaitForSignalObjectEx
//   3. WaitForMultipleObjectEx
//   4. SignalObjectAndWait
//   5. GetQueuedCompletionStatusEx
//   6. MsgWaitMultipelObjectEx

// 完成函数
VOID WINAPI OverlappedCompliteFun(DWORD dwErrorCode,  // 错误码
	DWORD dwNumberOfBytesTransfered,  // 成功读写的字节数
	LPOVERLAPPED lpOverlapped  // 重叠IO结构体指针
	)
{
	MyOVERLAPPED* pMyOverlapped = (MyOVERLAPPED*)lpOverlapped;


	printf("[%d] IO任务完成, 读取字节:%d, 读取位置:%d, 读取内容:[ %s ]\n",
		pMyOverlapped->nIndex,
		pMyOverlapped->InternalHigh,
		pMyOverlapped->Offset,
		pMyOverlapped->pBuff
		);


	// 释放空间
	delete  pMyOverlapped;

}

int main(int argc, char* argv[])
{
	HANDLE hFile = CreateFile(
		_T("main.cpp"),
		GENERIC_READ, // 打开为只读文件
		FILE_SHARE_READ,// 共享读权限
		NULL, // 安全描述符
		OPEN_EXISTING, // 打开时文件必须存在
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // 使用异步I/O
		NULL
		);

	if (hFile == INVALID_HANDLE_VALUE)
		return 0;

	// 第一个IO任务
	MyOVERLAPPED * pOv1 = new MyOVERLAPPED(50, 0);
	pOv1->nIndex = 0;
	ReadFileEx(hFile, pOv1->pBuff, 50, pOv1, OverlappedCompliteFun);

	// 第二个IO任务
	MyOVERLAPPED * pOv2 = new MyOVERLAPPED(80, 500);
	pOv2->nIndex = 1;
	ReadFileEx(hFile, pOv2->pBuff, 80, pOv2, OverlappedCompliteFun);

	SleepEx(100000, TRUE);
	system("pause");

	return 0;
}