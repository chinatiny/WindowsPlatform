#include <windows.h>
#include <stdio.h>

typedef struct _POSITION_
{
	DWORD dx;
	DWORD dz;
	DWORD dy;
}POSITION, *PPOSITION;

#define BASE_SIZE 0x1000
#define WALK_ADD 10
DWORD WINAPI WatchThread(LPVOID Param)
{
	ULONG size = 0;
	DWORD p = 0;
	UINT  Ret = 0;
	PVOID px[0x400] = { 0 };
	while (1)
	{
		size = 0x400;
		p = 0;
		Ret = GetWriteWatch(WRITE_WATCH_FLAG_RESET, Param, BASE_SIZE, px, &size, &p);
		if (Ret == 0
			&& size != 0)
		{
			MessageBoxW(NULL, L"发现第三方软件修改关键内存", L"发现第三方软件", MB_OK);
		}
	}
	return 0;
}
int __cdecl main(int argc, char *argv[])
{
	//申请Base基址内存
	PVOID Base = VirtualAlloc(NULL, BASE_SIZE, MEM_RESERVE | MEM_COMMIT | MEM_WRITE_WATCH, PAGE_READWRITE);
	PPOSITION pos = (PPOSITION)Base;
	//初始化坐标
	pos->dx = pos->dy = pos->dz = 1000;
	//清空监控记录
	ResetWriteWatch(Base, BASE_SIZE);
	//开启监控线程
	HANDLE hThread = CreateThread(NULL, 0, WatchThread, Base, 0, NULL);
	while (1)
	{
		//暂停监控线程
		SuspendThread(hThread);//也可以通过其他方式进行！！！！暂停线程只是简单模型
		//模拟走路过程
		pos->dx += WALK_ADD;
		//继续监控
		ResetWriteWatch(Base, BASE_SIZE);
		ResumeThread(hThread);
		//输出X坐标
		printf("x = %d\n", pos->dx);
		Sleep(1000);
	}
	return 0;
}