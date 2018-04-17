#include "MyOVERLAPPED.h"
#include <tchar.h>
#include <stdio.h>
#include <process.h>

// 等待IO事件有信号的函数
unsigned int __stdcall IOProc(void *pArg);

int main(int argc, char* argv[])
{
	HANDLE hFile = CreateFile(
		_T("main.cpp"),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		printf("打开文件失败, errcode:%d\n", GetLastError());
		return 1;
	}

	//设置第一个重叠io信息
	MyOVERLAPPED *ov1 = new MyOVERLAPPED;
	ov1->Offset = 0;
	ov1->OffsetHigh = 0;
	ov1->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	ov1->pBuff = new char[20];
	ZeroMemory(ov1->pBuff, 20);
	ov1->nIndex = 0;
	//派发io任务
	ReadFile(hFile, ov1->pBuff, 20, NULL, ov1);

	//设置第二个重叠io信息
	MyOVERLAPPED *ov2 = new MyOVERLAPPED(50, 300);
	ZeroMemory(ov2->pBuff, 50);
	ov2->nIndex = 1;
	ReadFile(hFile, ov2->pBuff, 50, NULL, ov2);



	// 创建线程等待IO任务的完成
	HANDLE hThreadEvent[2];
	hThreadEvent[0] = (HANDLE)_beginthreadex(0, 0, IOProc, ov1, 0, 0);
	hThreadEvent[1] = (HANDLE)_beginthreadex(0, 0, IOProc, ov2, 0, 0);

	WaitForMultipleObjects(_countof(hThreadEvent), hThreadEvent, TRUE, INFINITE);
	CloseHandle(hFile);

	system("pause");
	return 0;
}

// 等待事件信号的线程函数
unsigned int __stdcall IOProc(void *pArg)
{
	MyOVERLAPPED* pMyOverlapped = (MyOVERLAPPED*)pArg;

	WaitForSingleObject(pMyOverlapped->hEvent, -1);

	printf("[%d] IO任务完成, 读取字节:%d, 读取位置:%d, 读取内容:[ %s ]\n",
		pMyOverlapped->nIndex,
		pMyOverlapped->InternalHigh,
		pMyOverlapped->Offset,
		pMyOverlapped->pBuff
		);

	// 释放空间
	delete  pMyOverlapped;

	return 0;
}
