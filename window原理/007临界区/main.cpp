#include <windows.h>
#include <stdio.h>
#include <process.h>

int g_nNum = 0;
CRITICAL_SECTION g_criticalSection; // 临界区


unsigned int __stdcall ThreadProc(void *pArg)
{
	int nIndex = (int)pArg;
	// 进入临界区
	EnterCriticalSection(&g_criticalSection);

	++g_nNum;                                           //\ 
	Sleep(1); // 让线程进行切换.                         //| 这几行被包在临界区内的代码形成了原子操作 
	//| 只要没离开临界区,即使线程进行切换,其他线程不能执行代码
	printf("[%d] g_nNum = %d\n", nIndex, g_nNum);   /// 

	// 离开临界区
	LeaveCriticalSection(&g_criticalSection);

	return 0;
}



int main(int argc, char* argv[])
{

	// 初始化临界区
	InitializeCriticalSection(&g_criticalSection);

	HANDLE phThreadArray[10];
	for (int i = 0; i < 10; ++i) {

		// 创建线程
		phThreadArray[i] = (HANDLE)_beginthreadex(0, 0, ThreadProc, (void*)i, 0, 0);
	}


	// 等待所有线程返回
	WaitForMultipleObjects(10, phThreadArray, TRUE, INFINITE);

	system("pause");


	return 0;
}