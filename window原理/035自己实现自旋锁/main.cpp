#include <windows.h>
#include <process.h>
#include <stdio.h>


volatile int g_nSpinLockFlag = 0;

void __declspec(naked) AcquireSpinLock()
{
	__asm
	{
		pushfd;
	__begin:
		//bts 这个指令 : 把第0位的bit赋给cf
		lock bts dword ptr[g_nSpinLockFlag], 0;
		jb  __continue;  //cf == 1跳转
		popfd;
		ret
	__continue:
		test  dword ptr[g_nSpinLockFlag], 1;
		jz __begin;
		pause;
		jmp __continue;
	}
}

void __declspec(naked) ReleaseSpinLock()
{
	__asm
	{
		lock and  dword ptr[g_nSpinLockFlag], 0;
		ret;
	}
}

int g_num = 1;

unsigned int __stdcall ThreadProc1(void *pArg)
{

	while (true)
	{
		AcquireSpinLock();
		++g_num;
		printf("%d\n", g_num);
		ReleaseSpinLock();
	}
	return 0;

}

unsigned int __stdcall ThreadProc2(void *pArg)
{

	while (true)
	{
		AcquireSpinLock();
		++g_num;
		printf("%d\n", g_num);
		ReleaseSpinLock();
	}
	return 0;
}

unsigned int __stdcall ThreadProc3(void *pArg)
{

	while (true)
	{
		AcquireSpinLock();
		++g_num;
		printf("%d\n", g_num);
		ReleaseSpinLock();
	}
	return 0;
}


unsigned int __stdcall ThreadProc4(void *pArg)
{
	while (true)
	{
		AcquireSpinLock();
		++g_num;
		printf("%d\n", g_num);
		ReleaseSpinLock();
	}
	return 0;
}

int main(int argc, char* argv[])
{

	_beginthreadex(0, 0, ThreadProc1, 0, 0, 0);
	_beginthreadex(0, 0, ThreadProc2, 0, 0, 0);
	_beginthreadex(0, 0, ThreadProc3, 0, 0, 0);
	_beginthreadex(0, 0, ThreadProc4, 0, 0, 0);
	system("pause");

	return 0;
}