#include <windows.h>
#include <stdio.h>
#include "InlineHook.h"
#include <tchar.h>



void Func1()
{
	MessageBox(0, _T("原始函数"), _T("提示"), MB_OK);
}

void _stdcall HookedProc(HookContex* hookContex)
{
	TCHAR szHookBuff[MAX_PATH] = { 0 };

	_stprintf_s(szHookBuff, _countof(szHookBuff),
		_T("当执行的hook点的时候各个寄存器的值:\n\
		Eax = 0x%08X\n\
		Ecx = 0x%08X\n\
		Edx = 0x%08X\n\
		Ebx = 0x%08X\n\
		Esi = 0x%08X\n\
		Edi = 0x%08X\n\
		Ebp = 0x%08X\n\
		Esp = 0x%08X\n\
		EFlags = 0x%08X\n,你可以直接在这个函数中修改寄存器并在执行原来的地址的时候，寄存器将会是你修改的值\n"),
		hookContex->uEax,
		hookContex->uEcx,
		hookContex->uEdx,
		hookContex->uEbx,
		hookContex->uEsi,
		hookContex->uEdi,
		hookContex->uEbp,
		hookContex->uEsp,
		hookContex->uEflags
		);

	MessageBox(0, szHookBuff, _T("提示"), MB_OK);
}


int main()
{
	InlineHookSt inlineSt;
	InitInlineHook(&inlineSt, Func1, HookedProc);
	InstallInlineHook(&inlineSt);

	//测试安装hook的效果
	Func1();

	//卸载钩子的执行效果
	UninstallInlineHook(&inlineSt);
	Func1();
	return 0;
}
