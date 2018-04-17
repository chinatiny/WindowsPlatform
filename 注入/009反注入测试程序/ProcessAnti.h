#pragma once

//检测以阻塞的方式启动进程然后shellcode注入
void CheckSuspendCreateProcess();

//判断是否是非正常方式启动
bool IsNormalCreateProcess();