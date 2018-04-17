#include <ntifs.h>

//wrmsr  修改MSR寄存器
//事先把要修改的值放到EAX中，把要修改的号放到ECX中
//调用wrmsr，就会把eax的值写入到ecx对应的MSR寄存器中

//rdmsr  读取MSR寄存器
//把要读取的MSR的号，放到ECX中
//调用rdmsr就会把相应的MSR的值放到EAX中。

ULONG g_OldKiFastCallEntry;


PCLIENT_ID g_pClientPID;
PACCESS_MASK g_pAccessMask;
ULONG g_uSSDT_Index;
ULONG g_uProtectPID;
void _declspec(naked) MyKiFastCallEntry()
{
	// 1. 在EDX指向的用户空间栈中取出对应参数，以下是ZwOpen-
	//     Process的栈构造（暂时无需考虑其他调用函数的栈情况）。
	// +00 用户空间Call返回地址
	// +04 用户空间Call返回地址
	// +08 参数1 ( ProcessHandle：进程句柄 )
	// +12 参数2 ( DesiredAccess：访问权限 )
	// +16 参数3 ( ObjectName：对象名称 )
	// +20 参数4 ( ClientId：保存有进程PID与线程ID的结构体 )
	_asm push DWORD PTR[EDX + 4 * 5];	// 4*5 = 参数4
	_asm pop  g_pClientPID;	// 保存参数4到ClientPID
	_asm push EDX;
	_asm add  DWORD PTR[ESP], 4 * 3;	// 4*3 = 参数2
	_asm pop  g_pAccessMask;	// 保存参数2到AccessMask

	// 2. 保护指定PID的进程不被结束
	_asm pushad;
	// 2.1 判断否为ZwOpenProcess的调用号0xBE，是则执行保护
	_asm mov g_uSSDT_Index, eax;
	if (g_uSSDT_Index == 0xBE) {
		// 2.2 判断所操作进程是否为受保护进程，并同时判断访问权
		//     限中是否包含可结束进程权限PROCESS_TERMINATE，
		//     是的话修改其参数，去掉此权限
		if (((ULONG)g_pClientPID->UniqueProcess == g_uProtectPID))
		{
			*g_pAccessMask = 0;
		}
	}
	_asm popad;
	// 3. 跳转到系统原KiFastCallEntry函数中执行剩余操作
	_asm jmp g_OldKiFastCallEntry;
}

void init()
{
	_asm
	{
		push eax;
		push ecx;
		mov ecx, 0x176;
		rdmsr;
		mov g_OldKiFastCallEntry, eax;
		pop ecx;
		pop eax;
	}
}


void OnHook()
{
	_asm
	{
		push eax;
		push ecx;
		mov ecx, 0x176;
		mov eax, MyKiFastCallEntry;
		wrmsr;
		pop ecx;
		pop eax;
	}
}

void OffHook()
{
	_asm
	{
		push eax;
		push ecx;
		mov ecx, 0x176;
		mov eax, g_OldKiFastCallEntry;
		wrmsr;
		pop ecx;
		pop eax;
	}
}

VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	OffHook();
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	g_uProtectPID = 3824;
	init();
	OnHook();
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}

