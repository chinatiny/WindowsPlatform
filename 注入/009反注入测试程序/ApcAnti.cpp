#include "ApcAnti.h"
#include <windows.h>
#include <tchar.h>
#include "detours/detours.h"

#pragma comment(lib,"detours/lib.X86/detours.lib")

/*
	_NtQueueApcThread@20:
	7768FF34 B8 42 00 00 00       mov         eax,42h
	7768FF39 33 C9                xor         ecx,ecx
	7768FF3B 8D 54 24 04          lea         edx,[esp+4]
	7768FF3F 64 FF 15 C0 00 00 00 call        dword ptr fs:[0C0h]
	7768FF46 83 C4 04             add         esp,4
	7768FF49 C2 14 00             ret         14h

	; NTSTATUS __stdcall NtQueueApcThread(HANDLE ThreadHandle, PKNORMAL_ROUTINE ApcRoutine, PVOID NormalContext, PVOID SystemArgument1, PVOID SystemArgument2)
	public _NtQueueApcThread@20
	_NtQueueApcThread@20 proc near


	uac会执行loadlibaryA

	0x0276F970  96 4b 6e 77  ?Knw
	0x0276F974  00 00 3c 02  ..<.
	0x0276F978  35 a4 3e 75  5?>u
	0x0276F97C  d8 f9 76 02  ??v.
	0x0276F980  f4 fc 76 02  ??v.
	0x0276F984  00 00 00 00  ....
	0x0276F988  24 00 00 00  $...

	776E4B7B 83 FA FF             cmp         edx,0FFFFFFFFh
	776E4B7E 0F 84 2D 1E 01 00    je          _RtlDispatchAPC@12+11E5Dh (776F69B1h)
	776E4B84 8D 4D C4             lea         ecx,[ebp-3Ch]
	776E4B87 E8 85 D6 FA FF       call        @RtlActivateActivationContextUnsafeFast@8 (77692211h)
	776E4B8C 83 65 FC 00          and         dword ptr [ebp-4],0
	776E4B90 FF 75 0C             push        dword ptr [ebp+0Ch]
	776E4B93 FF 55 08             call        dword ptr [ebp+8]
	776E4B96 C7 45 FC FE FF FF FF mov         dword ptr [ebp-4],0FFFFFFFEh
	776E4B9D E8 A6 00 00 00       call        _RtlDispatchAPC@12+0F4h (776E4C48h)
	776E4BA2 E8 FA 93 FB FF       call        __SEH_epilog4 (7769DFA1h)
*/

//////////////////////////////类型定义///////////////////////////////////////
typedef NTSTATUS(WINAPI*fpNtQueueApcThread)(
	HANDLE ThreadHandle,
	PAPCFUNC ApcRoutine,
	PVOID NormalContext,
	PVOID SystemArgument1,
	PVOID SystemArgument2);

//apc回调必须调用的函数
typedef  int (WINAPI *fpRtlDispatchAPC)(
	int uknow1,
	int uknow2,
	PVOID Context);

//RtlAddVectoredExceptionHandler的地址减去0x6D得到RtlDispatchAPC地址
typedef PVOID(WINAPI *fpRtlAddVectoredExceptionHandler)(
	ULONG FirstHandler,
	PVECTORED_EXCEPTION_HANDLER VectoredHandler);


//////////////////////////////内部接口///////////////////////////////////////
static fpNtQueueApcThread   s_fpNtQueueApcThread = NULL;
static fpRtlAddVectoredExceptionHandler s_fpRtlAddVectoredExceptionHandler = NULL;
static fpRtlDispatchAPC s_fpRtlDispatchAPC = NULL;
static bool s_bQueryApcFlag = false;
static bool s_bFondApcInject = false;


//山寨版本的NtQueueApcThread
NTSTATUS WINAPI MyNtQueueApcThread(
	HANDLE ThreadHandle,
	PAPCFUNC ApcRoutine,
	PVOID NormalContext,
	PVOID SystemArgument1,
	PVOID SystemArgument2)
{
	s_bQueryApcFlag = true;
	return s_fpNtQueueApcThread(ThreadHandle, ApcRoutine, NormalContext, SystemArgument1, SystemArgument2);
}

//山寨版的RtlDispatchAPC
int WINAPI MyRtlDispatchAPC(
	int uknow1,
	int uknow2,
	PVOID Context)
{
	if (!s_bQueryApcFlag)
	{
		s_bFondApcInject = true;
	}
	s_bQueryApcFlag = false;
	return s_fpRtlDispatchAPC(uknow1, uknow2, Context);
}



//////////////////////////////对外接口///////////////////////////////////////
void MonitorApc()
{
	HMODULE hNtDll = LoadLibrary(_T("ntdll.dll"));
	s_fpNtQueueApcThread = (fpNtQueueApcThread)GetProcAddress(hNtDll, "NtQueueApcThread");
	s_fpRtlAddVectoredExceptionHandler = (fpRtlAddVectoredExceptionHandler)GetProcAddress(hNtDll, "RtlAddVectoredExceptionHandler");
	s_fpRtlDispatchAPC =(fpRtlDispatchAPC)((PBYTE)s_fpRtlAddVectoredExceptionHandler - 0x6D);


	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((PVOID*)&s_fpNtQueueApcThread, MyNtQueueApcThread);
	DetourAttach((PVOID*)&s_fpRtlDispatchAPC, MyRtlDispatchAPC);
	DetourTransactionCommit();
}

bool IsFondApcInject()
{
	return s_bFondApcInject;
}
