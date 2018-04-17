#pragma once
#include <windows.h>

//反程序运行时远程线程注入
void AntiRemoteThread();
//反程序运行之前外挂开启了线程
void CheckBeforOEPThread();
//是否发现非法代码
bool IsFondRemoteThread();