#include <windows.h>
#include <stdio.h>
#include "UnDocoumentApi.h"

bool IsDebug()
{
	// 获取TEB
	TEB* pTeb = NtCurrentTeb();

	// 获取PEB
	PEB* pPeb = pTeb->ProcessEnvironmentBlock;

	printf("NtGlobalFlag : %X\n", pPeb->NtGlobalFlag);

	// 获取是否正在调试字段的值.
	//检测PEB的NtGlobalFlag字段 , 如果被调试 , 此字段值为x70
	return pPeb->NtGlobalFlag == 0x70;

	int nResult = 0;
	_asm
	{
		push eax;
		push ebx;
		mov eax, FS:[0x30];//得到PEB
		xor ebx, ebx;
		mov ebx, [eax + 0x68]; // 检测PEB的NtGlobalFlag字段,如果被调试,此字段值为x70
		mov nResult, ebx;
		pop ebx;
		pop eax;
	}
	if (nResult == 0x70)
	{
		return true;
	}
	else
	{
		return false;
	}
}
int main(int argc, char* argv[])
{
	bool bIsDebug = IsDebug();
	if (bIsDebug)
	{
		MessageBoxA(NULL, "正在被调试", "警告", 0);
	}
	else
	{
		MessageBoxA(NULL, "现在很安全", "恭喜", 0);
	}
	return 0;
}