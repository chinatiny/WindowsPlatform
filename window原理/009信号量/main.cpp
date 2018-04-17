//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
// 信号量的使用
//
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <process.h>
#include <cstdio>
#include <time.h>

HANDLE  g_hSem;    // 信号量
HANDLE  g_hMutex;   // 互斥体
DWORD  g_Array[10];


#define COLOR_GREEN 0xa 
#define COLOR_WHITE 0xf 
// 带颜色打印字符串
void print(int Color, const char* pFormat, ...)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Color);

	va_list va;
	va_start(va, pFormat);
	vprintf(pFormat, va);
	va_end(va);
}



// 读者线程
unsigned int __stdcall ReaderProc(void* pArg)
{
	int nIndex = 0;
	while (true)
	{
		// 判断是否有写入信号
		// ReleaseSemaphore返回FALSE说明还没有写入数据的线程在工作
		if (FALSE == ReleaseSemaphore(g_hSem, 1, 0)) 
		{
			print(COLOR_WHITE, "[%d]没有数据读了\n", GetThreadId(GetCurrentThread()));
		}
		else 
		{
			// 写的时候不能读, 等待互斥体
			WaitForSingleObject(g_hMutex, -1);
			print(COLOR_WHITE, "[%d]读取数据:%d\n", GetThreadId(GetCurrentThread()), g_Array[0]);
			// 释放互斥体
			ReleaseMutex(g_hMutex);
		}

	}
	return 0;
}


// 写线程
unsigned int __stdcall WriterProc(void* pArg)
{
	DWORD nIndex = 0;
	while (true) 
	{
		// 开始要写入数据, 写和读不能同时进行, 等待互斥体.
		WaitForSingleObject(g_hMutex, INFINITE);
		g_Array[0] = rand() % 50;
		print(COLOR_GREEN, "[%d] --------- 写入数据:%d  --------- \n", GetThreadId(GetCurrentThread()), g_Array[0]);
		// 通知读线程有数据了
		WaitForSingleObject(g_hSem, INFINITE);
		// 释放互斥体
		ReleaseMutex(g_hMutex);
	}
	return 0;
}

int main()
{
	srand((unsigned int)time(NULL));

	g_hSem = CreateSemaphore(NULL,
		5,  // 有信号的个数
		5,  // 最大的信号个数
		NULL);

	g_hMutex = CreateMutex(NULL, FALSE, NULL);
	// 创建3个线程
	HANDLE phTread[3];
	phTread[0] = (HANDLE)_beginthreadex(0, 0, ReaderProc, 0, 0, 0);
	phTread[1] = (HANDLE)_beginthreadex(0, 0, ReaderProc, 0, 0, 0);
	phTread[2] = (HANDLE)_beginthreadex(0, 0, WriterProc, 0, 0, 0);

	WaitForMultipleObjects(_countof(phTread), phTread, TRUE, INFINITE);

	return 0;

}