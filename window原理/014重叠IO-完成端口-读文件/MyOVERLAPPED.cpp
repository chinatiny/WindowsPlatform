#include "MyOVERLAPPED.h"


// 默认构造函数
MyOVERLAPPED::MyOVERLAPPED()
:pBuff(NULL)
{
	OVERLAPPED::hEvent = 0;
}

//
MyOVERLAPPED::MyOVERLAPPED(int nIoSize, int nFileOffsetLow, int nFileOffsetHight /*= 0 */)
{
	// 创建事件对象
	OVERLAPPED::hEvent = 0;

	// 保存文件读写偏移
	OVERLAPPED::Offset = nFileOffsetLow;
	OVERLAPPED::OffsetHigh = nFileOffsetHight;

	// 申请缓冲区保存文件内容
	pBuff = new char[nIoSize];
}


// 析构函数,释放空间,释放事件对象
MyOVERLAPPED::~MyOVERLAPPED()
{
	// 释放空间
	if (pBuff != NULL) {
		delete[] pBuff;
		pBuff = NULL;
	}
}