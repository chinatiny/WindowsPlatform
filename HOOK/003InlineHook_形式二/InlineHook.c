#include "InlineHook.h"
#include "LDasm.h"

//
#define  MIN_MOVE_SIZE 0x6
#define  MAX_MOVE_SIZE BACKUPCODE_SIZE
#define  HOOK_ENTRY_SIZE 0x10



BYTE* HookAlloc(int nSize)
{
	BYTE *buff = NULL;
#ifdef _KERNEL_HOOK
	buff = ExAllocatePool(NonPagedPoolExecute, nSize);
#else
	buff = (BYTE*)VirtualAlloc(0, nSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
#endif 
	return buff;
}

void HookFree(BYTE* buff, int nSize)
{
#ifdef _KERNEL_HOOK
	ExFreePool(buff);
#else
	buff = (BYTE*)VirtualAlloc(0, nSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	VirtualFree(buff, nSize, MEM_DECOMMIT | MEM_RELEASE);
#endif 
}

//关闭内存写保护
ULONG HookOffMemProtect(LPVOID lpAddr, ULONG uSize)
{
	ULONG uOld = 0;
#ifdef _KERNEL_HOOK
	__asm
	{
		cli;
		push eax;
		mov eax, cr0;
		and eax, ~0x10000;
		mov cr0, eax;
		pop eax;
	}
#else
	VirtualProtect(lpAddr, uSize, PAGE_EXECUTE_READWRITE, &uOld);
#endif 
	return uOld;
}

//打开内存写保护
void HookOnMemProtect(LPVOID lpAddr, ULONG uSize, ULONG uOldValue)
{
#ifdef _KERNEL_HOOK
	__asm
	{ //恢复内存保护
		push eax;
		mov eax, cr0;
		or eax, 0x10000;
		mov cr0, eax;
		pop eax;
		sti;
	}
#else
	VirtualProtect(lpAddr, uSize, uOldValue, &uOldValue);
#endif 
}



BOOL InitInlineHook(InlineHookSt* inlineSt, PVOID lpHookAddr, fpTypeHookFun lpNewProc)
{
	//初始化数据
	inlineSt->lpHookAddr = lpHookAddr;
	//自动求出需要hook几个字节
	int pos = 0;
	int len = 0;
	do
	{
		len = SizeOfCode((BYTE*)lpHookAddr + pos, NULL);
		pos += len;
	} while (pos < MIN_MOVE_SIZE);
	//找不到好位置
	if (pos > MAX_MOVE_SIZE && pos < MIN_MOVE_SIZE)
	{
		inlineSt->bHookSucc = FALSE;
		return FALSE;
	}
	inlineSt->nOpcodeMove = pos;
	inlineSt->lpNewProc = lpNewProc;

	//备份旧的数据方便恢复
	memcpy(inlineSt->backupCode, lpHookAddr, inlineSt->nOpcodeMove);

	//构建HookEntry
	inlineSt->hookEntry = HookAlloc(HOOK_ENTRY_SIZE);
	inlineSt->hookEntry[0] = 0x60;  //pushad  0x60
	inlineSt->hookEntry[1] = 0x9C;  //pushfd 0x9c
	inlineSt->hookEntry[2] = 0x54;  //push esp
	inlineSt->hookEntry[3] = 0xE8; //call
	inlineSt->hookEntry[4] = 0x0;
	inlineSt->hookEntry[5] = 0x0;
	inlineSt->hookEntry[6] = 0x0;
	inlineSt->hookEntry[7] = 0x0;
	inlineSt->hookEntry[8] = 0x90; //nop
	inlineSt->hookEntry[9] = 0x9d;   //popfd   0x9d
	inlineSt->hookEntry[10] = 0x61;  //popad   0x61
	inlineSt->hookEntry[11] = 0xE9;   //jmp
	inlineSt->hookEntry[12] = 0x0;
	inlineSt->hookEntry[13] = 0x0;
	inlineSt->hookEntry[14] = 0x0;
	inlineSt->hookEntry[15] = 0x0;

	//求出inlineSt->hookEntry[3]到自己定义的山寨函数的偏移
	SIZE_T d1 = (SIZE_T)inlineSt->lpNewProc - (SIZE_T)&inlineSt->hookEntry[3] - 5;
	*(ULONG*)&inlineSt->hookEntry[4] = d1;

	//求出inlineSt->hookEntry[11]到我们自己定义的临时代码片段的偏移
	inlineSt->movedOpCode = HookAlloc(MAX_MOVE_SIZE);
	SIZE_T d2 = (SIZE_T)inlineSt->movedOpCode - (SIZE_T)&inlineSt->hookEntry[11] - 5;
	*(ULONG*)&inlineSt->hookEntry[12] = d2;
	inlineSt->bHookSucc = TRUE;
	return TRUE;
}

BOOL InstallInlineHook(InlineHookSt* inlineSt)
{
	//如果初始化失败直接返回
	if (!inlineSt->bHookSucc)
	{
		return inlineSt->bHookSucc;
	}

	//原始地址，也是第一行汇编指令地址
	BYTE* orginHookAddr = (BYTE*)inlineSt->lpHookAddr;
	//迁移原始指令片段到新内存空间
	//此处为指令迁移代码，需要设计到代码移位技术
	int nMove = (int)inlineSt->nOpcodeMove;
	BYTE* lpTemp = (BYTE*)inlineSt->movedOpCode;
	int len = 0;
	int pos = 0;
	do
	{
		len = SizeOfCode(orginHookAddr + pos, NULL);
		BYTE* codeIP = orginHookAddr + pos;
		//调整短jmc
		if ((BYTE)codeIP[0] >= 0x70 && (BYTE)codeIP[0] <= 0x7F)
		{
			WORD*	pJmpCode = (WORD*)(lpTemp);
			*pJmpCode = (codeIP[0] * 0x100) + 0x100F;
			// 0x70 ~ 0x7F   <=> 0x0f 0x71  ~ 0x0f 0x8F
			//T = S1 + 2 + D1 
			//T = S2 + 6 + D2
			//D2 = S1 + 2 + D1 - S2 - 6
			BYTE d1 = *(BYTE*)((ULONG)codeIP + 1);
			*(ULONG*)(lpTemp + 2) = (ULONG)codeIP + 2 + (ULONG)d1 - (ULONG)lpTemp - 6;
			lpTemp += 6;
		}
		else if ((BYTE)codeIP[0] == 0x0F && ((BYTE)codeIP[1] >= 0x80 && (BYTE)codeIP[1] <= 0x8F))
		{
			//调整长jmc
			*(WORD*)(lpTemp) = *(WORD*)codeIP;
			*(ULONG_PTR*)(lpTemp + 2) = *(ULONG_PTR*)((ULONG)codeIP + 2) + (ULONG)codeIP - (ULONG)lpTemp;
			//T = S1 + 6 + D1 
			//T = S2 + 6 + D2
			//D2 = S1 + 6 + D1 - S2 - 6
			//D2 = S1 + D1 - S2
			ULONG d1 = *(ULONG*)((ULONG)codeIP + 2);
			*(ULONG*)(lpTemp + 2) = (ULONG)codeIP + d1 - (ULONG)lpTemp;
			lpTemp += 6;
		}
		else if ((BYTE)codeIP[0] == 0xE9 || (BYTE)codeIP[0] == 0xE8)
		{
			//调整jmp,CALL
			//T = S1 + 5 + D1 
			//T = S2 + 5 + D2
			//D2 = S1 + 5 + D1 - S2 - 5
			//D2 = S1 + D1 - S2
			*(lpTemp) = codeIP[0];
			ULONG d1 = *(ULONG*)((ULONG)codeIP + 1);
			*(ULONG*)(lpTemp + 1) = (ULONG)codeIP + d1 - (ULONG)lpTemp;
			lpTemp += 5;
		}
		else
		{
			//其它指令直接复制
			memcpy((char*)(lpTemp), (char*)(orginHookAddr + pos), len);
			lpTemp += len;
		}
		pos += len;
	} while (pos < nMove);

	if (pos != nMove)
	{
		HookFree(inlineSt->hookEntry, HOOK_ENTRY_SIZE);
		HookFree(inlineSt->movedOpCode, MAX_MOVE_SIZE);
		inlineSt->bHookSucc = FALSE;
		return FALSE;
	}

	//在迁移后的指令片段后面添加跳转，跳转到原始指令主体
	BYTE* lpCode = (BYTE*)inlineSt->movedOpCode + pos;
	lpCode[0] = 0x68;
	*(ULONG_PTR*)&lpCode[1] = (ULONG_PTR)orginHookAddr + nMove;
	lpCode[5] = 0xC3;
	//HOOK代码指令
	ULONG uOldProtected = 0;
	uOldProtected = HookOffMemProtect(orginHookAddr, 0x1000);
	orginHookAddr[0] = 0x68;
	*(ULONG_PTR*)&orginHookAddr[1] = (ULONG_PTR)inlineSt->hookEntry;
	orginHookAddr[5] = 0xC3;
	HookOnMemProtect(orginHookAddr, 0x1000, uOldProtected);
	inlineSt->bHookSucc = TRUE;
	return TRUE;
}


VOID UninstallInlineHook(IN InlineHookSt* inlineSt)
{
	if (inlineSt->bHookSucc)
	{
		ULONG uOldProtected = 0;
		uOldProtected = HookOffMemProtect(inlineSt->lpHookAddr, 0x1000);
		memcpy(inlineSt->lpHookAddr, inlineSt->backupCode, inlineSt->nOpcodeMove);
		HookOnMemProtect(inlineSt->lpHookAddr, 0x1000, uOldProtected);
		//释放内存
		HookFree(inlineSt->hookEntry, HOOK_ENTRY_SIZE);
		HookFree(inlineSt->movedOpCode, MAX_MOVE_SIZE);
	}

}
