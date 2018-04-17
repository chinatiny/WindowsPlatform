#pragma once
#include <windows.h>


struct MyOVERLAPPED :public OVERLAPPED
{
public:
	char *pBuff; //缓冲区的首地址
	int nIndex;  //用于保存IO任务的序号

	MyOVERLAPPED();
	MyOVERLAPPED(int nIoSize, int nFileOffsetLow, int nFileOffsetHight = 0);
	~MyOVERLAPPED();
};