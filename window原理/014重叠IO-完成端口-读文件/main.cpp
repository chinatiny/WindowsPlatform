#include "MyOVERLAPPED.h"
#include <tchar.h>
#include <process.h>
#include <stdio.h>

// 等待完成端口通知的线程
unsigned int __stdcall WaitComplePortNotify(void * pArg);

int main(int argc, char* argv[])
{
	HANDLE hFile = CreateFile(
		_T("main.cpp"),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL
		);

	if (hFile == INVALID_HANDLE_VALUE)
		return 0;

	SYSTEM_INFO si = { 0 };
	GetSystemInfo(&si);

	//创建完成端口
	HANDLE hComplePort = CreateIoCompletionPort(
		INVALID_HANDLE_VALUE,
		NULL,
		0,
		2 * si.dwNumberOfProcessors);

	//将文件对象和完成端口进行关联
	CreateIoCompletionPort(
		hFile,
		hComplePort,
		0,
		0);

	// 创建等待完成端口通知的线程, 可以只创建1个线程,
	// 为了更高的效率, 这里创建了CPU个数2倍个线程.
	for (unsigned int i = 0; i < si.dwNumberOfProcessors * 2; ++i)
		_beginthreadex(0, 0, WaitComplePortNotify, (void*)hComplePort, 0, 0);

	//////////////////////////////////////////////////////////////////////////
	// 开始投递IO任务

	// 开始投递IO任务
	MyOVERLAPPED *pOv1 = new MyOVERLAPPED(100, 0);
	pOv1->nIndex = 1;
	ReadFile(hFile, pOv1->pBuff, 100, NULL, pOv1);


	// 开始投递IO任务
	MyOVERLAPPED *pOv2 = new MyOVERLAPPED(100, 200);
	pOv2->nIndex = 2;
	ReadFile(hFile, pOv2->pBuff, 100, NULL, pOv2);



	// 开始投递IO任务
	MyOVERLAPPED *pOv3 = new MyOVERLAPPED(100, 400);
	pOv3->nIndex = 3;
	ReadFile(hFile, pOv3->pBuff, 100, NULL, pOv3);


	// 开始投递IO任务
	MyOVERLAPPED *pOv4 = new MyOVERLAPPED(100, 600);
	pOv4->nIndex = 4;
	ReadFile(hFile, pOv4->pBuff, 100, NULL, pOv4);

	system("pause");
	// 将一个IO完成状态发送到完成端口
	PostQueuedCompletionStatus(hComplePort, /* 接收完成状态的完成端口的句柄 */
		0, /* 完成了多少个字节 */
		0, /* 完成键 */
		0   /* 重叠结构体变量地址 */
		);
	return 0;
}

unsigned int __stdcall WaitComplePortNotify(void * pArg)
{
	HANDLE hComplePort = (HANDLE)pArg;

	DWORD           dwCompleSize = 0;// IO任务完成的字节数 
	DWORD           dwCompleKey = 0; // 完成键(在这里没啥用)
	MyOVERLAPPED*   pMyOverlapped = NULL;// OVERLAPPED结构体地址

	BOOL            bRet = FALSE;
	while (true) {

		// 等待IO任务被投递到完成队列
		bRet = GetQueuedCompletionStatus(hComplePort,
			&dwCompleSize,
			&dwCompleSize,
			(OVERLAPPED**)&pMyOverlapped,
			INFINITE
			);


		// 判断是否真的获取到了已经完成的IO任务
		if (bRet == TRUE && pMyOverlapped != NULL) {

			printf("[%d] IO任务完成, 读取字节:%d, 读取位置:%d, 读取内容:[ %s ]\n",
				pMyOverlapped->nIndex,
				pMyOverlapped->InternalHigh,
				pMyOverlapped->Offset,
				pMyOverlapped->pBuff
				);
		}

		delete pMyOverlapped;
	}
}