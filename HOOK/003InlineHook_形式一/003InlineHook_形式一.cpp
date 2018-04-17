// 003InlineHook_形式一.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include <windows.h>

BYTE	g_jmpShellCode[5] = { "\xe9" };
BYTE	g_oldOpcode[5] = { 0 };

// 盗版的MessageBox
DWORD WINAPI MyMessageBox(HWND hWnd, TCHAR* pText, TCHAR* pTitle, DWORD type)
{

	DWORD dwOldProtect = 0;
	DWORD dwWrite = 0;
	VirtualProtectEx(GetCurrentProcess(),
		&MessageBox,
		4,
		PAGE_EXECUTE_READWRITE,
		&dwOldProtect
		);


	// 恢复函数原来的内容
	WriteProcessMemory(GetCurrentProcess(),
		&MessageBox,
		g_oldOpcode,
		sizeof(g_jmpShellCode),
		&dwWrite
		);

	// 调用原版函数
	MessageBox(0, L"在盗版的MessageBox中弹出此框", L"提示", 0);



	// 调用完原版函数之后,再次HOOK这个函数
	WriteProcessMemory(GetCurrentProcess(),
		&MessageBox,
		g_jmpShellCode,
		sizeof(g_jmpShellCode),
		&dwWrite
		);

	//	6. 恢复内存分页的属性
	VirtualProtectEx(GetCurrentProcess(),
		&MessageBox,
		4,
		dwOldProtect,
		&dwOldProtect
		);

	return 0;
}

// 盗版的MessageBox
DWORD WINAPI MyMessageBox2(HWND hWnd, TCHAR* pText, TCHAR* pTitle, DWORD type) {
	//MessageBox( hWnd , pText , pTitle , type );
	MessageBox(hWnd, L"--------------", pTitle, type);
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{

	// 内联HOOK的示意:
	// 未被拦截的函数
	// MessageBox:
	//		+--------------+
	// ==>  | mov edi,edi  |
	//		| push ebp     |
	//		| mov ebp,esp  |
	//		|              |
	// 		|  XXXXXXXXX   |
	//		+--------------+
	// 
	// 被拦截的函数	
	//										function
	//	MessageBox:				  +--->>-------------------+			
	//		+--------------+	  |	   |	push ebp	   |
	// ==>  | jmp function>>------+    |   mov ebp,esp     |
	//		| push ebp     |		   |                   |
	//		| mov ebp,esp  |		   |     XXXXXX        |
	//		|              |		   |     XXXXXX        |
	// 		|  XXXXXXXXX   |		   |     XXXXXX        |
	//		+--------------+		   |     XXXXXX        |
	//								   | jmp  MessageBox+5 |
	//								   +-------------------+
	//
	// 所谓HOOK, 就是拦截, HOOK API就是在API的真正代码被执行前拦截它
	// 跳转到另一个地方执行代码, 执行后再跳转回去,或不跳转.
	// HOOK的概念比简单, 但一些细节需要注意
	// 例如:
	// 1. 原函数是没有跳转到XXX地址的代码的,jmp XXX是我们通过修改内存
	//	  加入进去的, 在修改内存时,需要注意的是,一般能够被执行的内存分页
	//    都没有可写的属性,因此,在修改内存前,需要修改内存分页属性.
	// 2. jmp 指令需要用到一个跳转偏移, 而非一个绝对的地址. 将jmp XXX的
	//    opcode写入到内存时, 必须知道怎么计算出这个跳转偏移. 一般跳转
	//    偏移需要用到以下公式来计算:
	//		跳转偏移 = 目标地址 - 当前跳转指令所在地址 - 跳转指令的总体长度
	//	  如:  
	//		指令要跳转到 011AB61F
	//		jmp 011AB61F 这条跳转指令所在地址为 011AB618
	//		jmp 011AB61F 这条指令的总体长度为2, 因为它的opcode是EB 05,只有2个字节
	//		所以它的偏移为: 011AB61F - 011AB61F - 2  => 5
	//		   
	//	+-<011AB618 | EB 05 | JMP 011AB61F 
	//	|  011AB61A | 31 C0 | XOR EAX , EAX 
	//	|  011AB61C | 31 DB | XOR EBX , EBX 
	//	|  011AB61E | 40    | INC EAX 
	//	+> 011AB61F | 43    | INC EBX 


	// 内联HOOK步骤:
	//	1. 设置要修改的内存的分页属性为可写
	//  2. 准备跳转的shellcode
	//  3. 计算跳转偏移,并将计算好的跳转偏移写入到shellcode中
	//  4. 将函数开始地址处的opcode备份,字节数和shellcode等长.
	//	5. 将opcode写入到要HOOK的函数
	//	6. 恢复内存分页的属性

	// 1. 设置要修改的内存的分页属性为可写
	DWORD dwOldProtect = 0;
	VirtualProtectEx(GetCurrentProcess(),
		&MessageBox,
		4,
		PAGE_EXECUTE_READWRITE,
		&dwOldProtect
		);

	// 2. 准备跳转的shellcode
	BYTE	g_jmpShellCode[5] = { "\xe9" };


	// 3. 计算跳转偏移,并将计算好的跳转偏移写入到shellcode中
	DWORD dwJmpOffset = (DWORD)&MyMessageBox - (DWORD)&MessageBoxW - 5;
	*(DWORD*)(g_jmpShellCode + 1) = dwJmpOffset;



	// 4. 将函数开始地址处的opcode备份,字节数和shellcode等长.
	//    BYTE	g_oldOpcode[ 5 ] = { 0 };
	DWORD	dwRead = 0;
	ReadProcessMemory(GetCurrentProcess(),
		&MessageBox,
		g_oldOpcode,
		sizeof(g_jmpShellCode),
		&dwRead
		);


	//	5. 将opcode写入到要HOOK的函数
	WriteProcessMemory(GetCurrentProcess(),
		&MessageBox,
		g_jmpShellCode,
		sizeof(g_jmpShellCode),
		&dwRead
		);

	//	6. 恢复内存分页的属性
	VirtualProtectEx(GetCurrentProcess(),
		&MessageBox,
		4,
		dwOldProtect,
		&dwOldProtect
		);


	// 函数被调用后,将跑到钩子函数中执行代码
	MessageBox(NULL, L"我是正版MessageBox", L"提示", 0);
	return 0;
}



