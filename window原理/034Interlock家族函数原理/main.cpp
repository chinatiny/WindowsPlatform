#include <windows.h>
#include <process.h>
#include <stdio.h>


volatile LONG g_lValue = 1;
volatile LONG g_lTarget = 2;

int main(int argc, char* argv[])
{
	/*
	.text:7DD71350 ; LONG __stdcall InterlockedIncrement(volatile LONG *lpAddend)
	.text:7DD71350 _InterlockedIncrement@4 proc near       ; CODE XREF: InterlockedIncrementStub(x)+6↓j
	.text:7DD71350                                         ; RegKrnInitialize(x,x,x)+36F8↓p ...
	.text:7DD71350
	.text:7DD71350 lpAddend        = dword ptr  4
	.text:7DD71350
	.text:7DD71350                 mov     ecx, [esp+lpAddend]
	.text:7DD71354                 mov     eax, 1
	说明:把eax加到[ecx]中,并且把[ecx]原来的值交换给eax
	.text:7DD71359                 lock xadd [ecx], eax
	.text:7DD7135D                 inc     eax
	.text:7DD7135E                 retn    4
	.text:7DD7135E _InterlockedIncrement@4 endp
	*/
	InterlockedIncrement(&g_lValue);

	/*
	text:7DD71364 ; LONG __stdcall InterlockedDecrement(volatile LONG *lpAddend)
	.text:7DD71364 _InterlockedDecrement@4 proc near       ; CODE XREF: InterlockedDecrementStub(x)+6↓j
	.text:7DD71364                                         ; RegKrnInitialize(x,x,x)+4833↓p ...
	.text:7DD71364
	.text:7DD71364 lpAddend        = dword ptr  4
	.text:7DD71364
	.text:7DD71364                 mov     ecx, [esp+lpAddend]
	.text:7DD71368                 mov     eax, 0FFFFFFFFh
	说明:把eax加到[ecx]中,并且把[ecx]原来的值交换给eax
	.text:7DD7136D                 lock xadd [ecx], eax
	.text:7DD71371                 dec     eax
	.text:7DD71372                 retn    4
	.text:7DD71372 _InterlockedDecrement@4 endp
	*/
	InterlockedDecrement(&g_lValue);

	/*
	text:7DD71378 ; LONG __stdcall InterlockedExchange(volatile LONG *Target, LONG Value)
	.text:7DD71378 _InterlockedExchange@8 proc near        ; CODE XREF: InterlockedExchangeStub(x,x)+6↓j
	.text:7DD71378
	.text:7DD71378 Target          = dword ptr  4
	.text:7DD71378 Value           = dword ptr  8
	.text:7DD71378
	.text:7DD71378                 mov     ecx, [esp+Target]
	.text:7DD7137C                 mov     edx, [esp+Value]
	.text:7DD71380                 mov     eax, [ecx]
	.text:7DD71382
	.text:7DD71382 loc_7DD71382:                           ; CODE XREF: InterlockedExchange(x,x)+E↓j
	.text:7DD71382                 lock xchg [ecx], edx
	.text:7DD71388                 retn    8
	.text:7DD71388 _InterlockedExchange@8 endp
	*/
	LONG lValue = 1;
	InterlockedExchange(&g_lTarget, lValue);


	/*
	text:7DD7138C ; LONG __stdcall InterlockedCompareExchange(volatile LONG *Destination, LONG Exchange, LONG Comperand)
	.text:7DD7138C _InterlockedCompareExchange@12 proc near
	.text:7DD7138C                                         ; CODE XREF: GetStartupInfoA(x)+122↑p
	.text:7DD7138C                                         ; InterlockedCompareExchangeStub(x,x,x)+6↓j ...
	.text:7DD7138C
	.text:7DD7138C Destination     = dword ptr  4
	.text:7DD7138C Exchange        = dword ptr  8
	.text:7DD7138C Comperand       = dword ptr  0Ch
	.text:7DD7138C
	.text:7DD7138C                 mov     ecx, [esp+Destination]
	.text:7DD71390                 mov     edx, [esp+Exchange]
	.text:7DD71394                 mov     eax, [esp+Comperand]
	说明:
	比较eax和[ecx]的值:
	相等:  [ecx] <-  edx
	不相等: eax <- [ecx]
	.text:7DD71398                 lock cmpxchg [ecx], edx
	.text:7DD7139C                 retn    0Ch
	*/

	//1.情况1,[ecx]  == eax
	// target:会成为1
	// lResult:会成为9
	LONG lExchange = 1, lcomperand = 9;
	g_lTarget = 9;
	LONG lResult = InterlockedCompareExchange(&g_lTarget, lExchange, lcomperand);
	printf("lresult:%d\n", lResult);

	//2. 情况2:[ecx] != eax
	//target:不变是5
	//lResult: lesult <- g_lTarget = 5
	lExchange = 1;
	lcomperand = 9;
	g_lTarget = 5;
	lResult = InterlockedCompareExchange(&g_lTarget, lExchange, lcomperand);
	printf("lresult:%d\n", lResult);


	/*
	text:7DD713A0 ; LONG __stdcall InterlockedExchangeAdd(volatile LONG *Addend, LONG Value)
	.text:7DD713A0 _InterlockedExchangeAdd@8 proc near     ; CODE XREF: InterlockedExchangeAddStub(x,x)+6↓j
	.text:7DD713A0
	.text:7DD713A0 Addend          = dword ptr  4
	.text:7DD713A0 Value           = dword ptr  8
	.text:7DD713A0
	.text:7DD713A0                 mov     ecx, [esp+Addend]
	.text:7DD713A4                 mov     eax, [esp+Value]
	说明:增加后返回原来的值
	.text:7DD713A8                 lock xadd [ecx], eax
	.text:7DD713AC                 retn    8
	.text:7DD713AC _InterlockedExchangeAdd@8 endp
	*/

	g_lTarget = 5;
	lResult = InterlockedExchangeAdd(&g_lTarget, 1);
	printf("lresult:%d\n", lResult);

	return 0;
}