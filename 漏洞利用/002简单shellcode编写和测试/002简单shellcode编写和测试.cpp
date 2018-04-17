// 练习2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>



int _tmain(int argc, _TCHAR* argv[])
{
	//shell code 编码
	__asm
	{
		pushad;
		sub esp, 0x20;
		jmp tag_ShellCode;

		//  [tag_Next - 0x52] "GetProcAddress"
		_asm _emit(0x47)  _asm _emit(0x65)   _asm _emit(0x74)  _asm _emit(0x50)
		_asm _emit(0x72)  _asm _emit(0x6F)   _asm _emit(0x63)  _asm _emit(0x41)
		_asm _emit(0x64)  _asm _emit(0x64)   _asm _emit(0x72)  _asm _emit(0x65)
		_asm _emit(0x73)  _asm _emit(0x73)

		//  [tag_Next - 0x44] "LoadLibraryExA"
		_asm _emit(0x4C)  _asm _emit(0x6F)   _asm _emit(0x61)  _asm _emit(0x64)
		_asm _emit(0x4C)  _asm _emit(0x69)   _asm _emit(0x62)  _asm _emit(0x72)
		_asm _emit(0x61)  _asm _emit(0x72)   _asm _emit(0x79)  _asm _emit(0x45)
		_asm _emit(0x78)  _asm _emit(0x41)  _asm _emit(0x00)

		//  [tag_Next - 0x35] "User32.dll"
		_asm _emit(0x55)  _asm _emit(0x73)   _asm _emit(0x65)  _asm _emit(0x72)
		_asm _emit(0x33)  _asm _emit(0x32)   _asm _emit(0x2E)  _asm _emit(0x64)
		_asm _emit(0x6C)  _asm _emit(0x6C)   _asm _emit(0x00)

		//  [tag_Next - 0x2A] "MessageBoxA"
		_asm _emit(0x4D)  _asm _emit(0x65)   _asm _emit(0x73)  _asm _emit(0x73)
		_asm _emit(0x61)  _asm _emit(0x67)   _asm _emit(0x65)  _asm _emit(0x42)
		_asm _emit(0x6F)  _asm _emit(0x78)   _asm _emit(0x41)  _asm _emit(0x00)

		//  [tag_Next - 0x1E] "ExitProcess"
		_asm _emit(0x45)  _asm _emit(0x78)   _asm _emit(0x69)  _asm _emit(0x74)
		_asm _emit(0x50)  _asm _emit(0x72)   _asm _emit(0x6F)  _asm _emit(0x63)
		_asm _emit(0x65)  _asm _emit(0x73)   _asm _emit(0x73)  _asm _emit(0x00)

		//  [tag_Next - 0x12] "Hello World"
		_asm _emit(0x48)  _asm _emit(0x65)   _asm _emit(0x6C)  _asm _emit(0x6C)
		_asm _emit(0x6F)  _asm _emit(0x20)   _asm _emit(0x31)  _asm _emit(0x35)
		_asm _emit(0x50)  _asm _emit(0x42)   _asm _emit(0x21)  _asm _emit(0x00)

	tag_ShellCode:
		//1. GetPc
		call tag_Next;
		tag_Next :
		pop ebx; // ebx == BaseAddr

		//2 获取关键模块地址
		mov esi, dword ptr fs : [0x30];
		mov esi, [esi + 0x0C];
		mov esi, [esi + 0x1C];
		mov esi, [esi];
		mov edx, [esi + 0x08];   //edx为Kernel32.dll的地址

		//3. 获取GetProcAddress的函数地址
		push ebx;
		push edx;
		call fun_GetProcAddress;
		mov esi, eax;

		//4. 获取LoadLibaryExA的地址
		lea  ecx, [ebx - 0x43];   // loadlibaryExa的字符串
		push ecx;
		push edx;
		call eax;

		//5. 调用payload部分
		push ebx;   // baseaddr
		push esi;    // Getprocess
		push  eax;  //loadlibraryexa
		push edx;  // kernel32 基地址
		call fun_PayLoad;



	fun_GetProcAddress:  //(imageBase, BaseAddr)
		push ebp;
		mov ebp, esp;
		sub esp, 0x0C;
		push edx;

		//1. 获取eax,ent, ept的地址
		mov edx, [ebp + 0x8];  // kernel32.dll
		mov esi, [edx + 0x3C];  // image_dos_header.e_lfanew
		lea esi, [edx + esi];   // pe文件头的va
		mov esi, [esi + 0x78]; // esi = 导出表的rva
		lea esi, [edx + esi];  //esi = 导出表的va
		mov edi, [esi + 0x1C];  //edi 导出地址表的rva
		lea edi, [edx + edi];  // edi 导出地址表的va
		mov[ebp - 0x4], edi;  // [ebp - 0x4]存放的是导出地址的va
		mov edi, [esi + 0x20];  //edi =导出名字表的rva
		lea edi, [edx + edi];   // edi =导出名字表的va
		mov[ebp - 0x8], edi; // [ebp - 0x8]等于导出名字表
		mov edi, [esi + 0x24]; // edi 导出序号表的rva
		lea edi, [edx + edi]; // edi 是导出序号表的va
		mov[ebp - 0xC], edi;  //[ebp - 0xc]是导出序号表的va

			//2. 循环对比ent中的函数名
		xor eax, eax;
		jmp tag_FistCmp;
	tag_CmpFunNameLoop:
		inc eax;
	tag_FistCmp:
		mov esi, [ebp - 0x8];  // esi == 导出名字的表的va
		mov esi, [esi + 4 * eax];
		mov edx, [ebp + 0x8];  // 取出imagebase
		lea esi, [edx + esi];  //获取名字
		mov ebx, [ebp + 0xC];  // BaseAddr
		lea edi, [ebx - 0x51];  // edi == "GetProcAddress"
		mov ecx, 0x0E;
		cld;
		repe cmpsb;
		jne tag_CmpFunNameLoop;

		//3. 成功后找到对应的序号
		mov esi, [ebp - 0xC];   // esi = Eot
		xor edi, edi;
		mov di, [esi + eax * 2];

		//4. 使用序号作为索引找到函数名所对应的函数地址
		mov edx, [ebp - 0x4];   // eat
		mov esi, [edx + edi * 4]; // 用序号找到函数的rva
		mov edx, [ebp + 0x8]; //ImageBase

		//5. 返回获取关键函数地址
		lea eax, [edx + esi];  // edx== imagebase, esi为序号

		pop edx;
		mov esp, ebp;
		pop ebp;
		retn 0x8;


	fun_PayLoad: //(Kernel32_base, LoadLibraryExA, GetProcAddress, BaseAddress)
		push ebp;
		mov ebp, esp;
		sub esp, 0x8;
		mov ebx, [ebp + 0x14]; // ebx = Param_4

		//1. 获取MessageBox的地址
		lea ecx, [ebx - 0x34];
		push 0;
		push 0;
		push ecx;  // "user32.dll"
		call[ebp + 0xC];  // loadlibarayExA

		lea ecx, [ebx - 0x29];   //messageboxA
		push ecx;
		push eax;  // user32的地址
		call [ebp + 0x10];   //GetProcAddress
		mov[ebp - 0x4], eax;  //[ebp - 4]存放的是MessageBoxA的地址

		//2. 获取ExitProcess的地址
		lea ecx, [ebx - 0x1D];  // "ExitProcess"
		push ecx;
		push[ebp + 0x8];  // kernel32基地址
		call[ebp + 0x10];  //GetprocAddress
		mov[ebp - 0x8], eax;

		//3. 显示hello 15pb
		lea  ecx, [ebx - 0x11]; //"hello 15pb"
		push 0;
		push ecx;
		push ecx;
		push 0;
		call[ebp - 0x4];

		//4. 执行退出
		push 0;
		call[ebp - 0x8];

		mov esp, ebp;
		pop ebp;
		retn 0x10;
	}
	printf("防止优化\n");
	system("pause");
	return 0;
}

