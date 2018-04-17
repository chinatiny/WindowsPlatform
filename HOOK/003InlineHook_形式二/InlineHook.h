#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#ifdef _KERNEL_HOOK
#include <minwindef.h>
#else
#include <windows.h>
#endif

#define  BACKUPCODE_SIZE  0x200
	/*
	1. hook函数执行的时候，各个寄存器的环境，可以通过直接修改这些值对寄存器做过滤
	2. 可以根据esp对参数进行过滤
	*/
	typedef struct _HookContex
	{
		ULONG uEflags;
		ULONG uEdi;
		ULONG uEsi;
		ULONG uEbp;
		ULONG uEsp;
		ULONG uEbx;
		ULONG uEdx;
		ULONG uEcx;
		ULONG uEax;
	}HookContex;

	//hook函数的指定的类型
	typedef void(_stdcall *fpTypeHookFun)(HookContex* hookContex);
	//inlinehook的结构体，不用关注这些细节，直接使用初始化函数自动会帮我们填充,所以不用关注这个结构体
	typedef  struct  _InlineHookSt
	{
		PVOID lpHookAddr;   //被hook的地址
		int nOpcodeMove; //从lpHookAddr点开始计算，需要移动多少指令到movedOpCode
		BYTE backupCode[BACKUPCODE_SIZE];  //当hook恢复的时候,用来还原的指令
		//
		BYTE*  hookEntry;   //hook入口
		BYTE* movedOpCode; //移动的opcode的缓冲区
		fpTypeHookFun lpNewProc;  //我们的函数地址
		BOOL bHookSucc; //hook是否成功
	}InlineHookSt;


	/*
	参数说明：
	    inlineSt： inline hook的结构体,直接传入一个结构体的指针即可，其他初始化函数会帮我们填充
	    lpHookAddr： 任意汇编地址开始的地方
	    lpNewProc： 新的函数地址
	*/
	BOOL InitInlineHook(OUT InlineHookSt* inlineSt, IN PVOID lpHookAddr, IN fpTypeHookFun lpNewProc);


	/*
	参数说明：
	    inlineSt： 直接传入InitInlineHook初始化的inlineSt即可
	*/
	BOOL InstallInlineHook(IN InlineHookSt* inlineSt);


	/*
	参数说明：
	    inlineSt： 直接传入InstallInlineHook使用的inlineSt即可
	*/
	VOID UninstallInlineHook(IN InlineHookSt* inlineSt);

#ifdef __cplusplus
}
#endif


