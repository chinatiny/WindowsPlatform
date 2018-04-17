#pragma once

#include <windows.h>


#ifdef MESSAGEHOOKDLL_EXPORTS
#define MESSAGEHOOKDLL_API extern"C" __declspec(dllexport)
#else
#define MESSAGEHOOKDLL_API extern"C" __declspec(dllimport)
#endif

// 安装钩子
MESSAGEHOOKDLL_API int InstallHook();

// 卸载钩子
MESSAGEHOOKDLL_API int UninstallHook();

//需要挂钩的线程id
MESSAGEHOOKDLL_API extern  DWORD g_tid;