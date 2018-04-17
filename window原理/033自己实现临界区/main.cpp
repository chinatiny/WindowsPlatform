#include <windows.h>
#include <stdio.h>
#include <process.h>

volatile LONG g_lCsFlag = 0;

__declspec(naked) void EnterCs()
{
	__asm
	{
		push eax
	__LAB:
		mov eax, 1;
		lock xadd dword ptr[g_lCsFlag], eax;
		cmp eax, 0;
		jz __END_LABLE;
		lock dec dword ptr[g_lCsFlag];
		pause; //ΩµŒ¬÷∏¡Ó
		jmp __LAB;
	__END_LABLE:
		pop eax
		ret
	}
}

__declspec(naked) void LeaveCs()
{
	__asm
	{
		lock dec dword ptr[g_lCsFlag];
		ret
	}
}

int g_num = 1;

unsigned int __stdcall ThreadProc1(void *pArg)
{

	while (true)
	{
		EnterCs();
		++g_num;
		printf("%d\n", g_num);
		LeaveCs();
	}
	return 0;

}

unsigned int __stdcall ThreadProc2(void *pArg)
{

	while (true)
	{
		EnterCs();
		++g_num;
		printf("%d\n", g_num);
		LeaveCs();
	}
	return 0;
}

unsigned int __stdcall ThreadProc3(void *pArg)
{

	while (true)
	{
		EnterCs();
		++g_num;
		printf("%d\n", g_num);
		LeaveCs();
	}
	return 0;
}


unsigned int __stdcall ThreadProc4(void *pArg)
{
	while (true)
	{
		EnterCs();
		++g_num;
		printf("%d\n", g_num);
		LeaveCs();
	}
	return 0;
}



int main(int argc, char * argv)
{
	_beginthreadex(0, 0, ThreadProc1, 0, 0, 0);
	_beginthreadex(0, 0, ThreadProc2, 0, 0, 0);
	_beginthreadex(0, 0, ThreadProc3, 0, 0, 0);
	_beginthreadex(0, 0, ThreadProc4, 0, 0, 0);
	system("pause");
	return 0;
}