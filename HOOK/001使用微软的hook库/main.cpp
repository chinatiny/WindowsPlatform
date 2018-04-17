#include <windows.h>
#include <stdio.h>

//1. 包含头文件
#include "detours/detours.h"

//2. 包含静态库
#ifdef _X64
#pragma comment(lib,"detours/lib.X64/detours.lib")
#else
#pragma comment(lib,"detours/lib.X86/detours.lib")
#endif // _X64

typedef int (WINAPI *fnMessageBox)(
_In_opt_ HWND hWnd,
_In_opt_ LPCWSTR lpText,
_In_opt_ LPCWSTR lpCaption,
_In_ UINT uType);

fnMessageBox g_pSrcMessageBoxW = NULL;

DWORD WINAPI MyMessageBox(HWND hWnd, TCHAR* pText, TCHAR* pTitle, DWORD type) 
{
	g_pSrcMessageBoxW(0, L"在盗版的MessageBox中弹出此框", L"提示", 0);
	return 0;
}



int main(int argc, char* argv[])
{

	MessageBoxW(0, L"正版函数", L"提示", 0);

	//3. 执行hook
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	g_pSrcMessageBoxW = MessageBoxW;
	DetourAttach((LPVOID*)&g_pSrcMessageBoxW, MyMessageBox);
	// 提交所有的更改(所有的HOOK)
	if (DetourTransactionCommit() == NO_ERROR)
	{
		printf("hook成功");
	}

	//4. 测试hook效果
	MessageBoxW(0, L"正版函数", L"提示", 0);


	//5. 卸载钩子
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach((LPVOID*)&g_pSrcMessageBoxW, &MyMessageBox);
	DetourTransactionCommit();


	//6.在次测试
	MessageBoxW(0, L"正版函数", L"提示", 0);



	return 0;
}