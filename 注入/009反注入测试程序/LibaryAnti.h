#pragma once

//监控加载pe模块
void MonitorLoadDll();
//获取是否由shellcode注入
bool IsFondShellCode();
//获取dll是否有隐藏自己的行为
bool IsFondModHidSelf();

