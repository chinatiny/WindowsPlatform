#include <windows.h>


int main(int argc, char* argv[])
{
	// API内部实际是监测PEB的BeingDebugged字段
	BOOL bIsDebug = IsDebuggerPresent();

	/*
		76B938F0 64 A1 18 00 00 00    mov         eax, dword ptr fs : [00000018h]
		76B938F6 8B 40 30             mov         eax, dword ptr[eax + 30h]
		76B938F9 0F B6 40 02          movzx       eax, byte ptr[eax + 2]
		*/

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