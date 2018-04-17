#include <windows.h>
#include <TlHelp32.h>

//遍历进程函数,返回true结束遍历，返回false继续遍历
typedef bool(*pfTypeEnumProcess)(IN  void *callBackParameter, IN PROCESSENTRY32* pProcessEntry);
/*
	作用：遍历进程
	参数：
		fpEnum ：遍历进程使用的回调函数
		callBackParameter：遍历进程传递个回调函数的参数
	返回值：如果返回true代表正常返回，否则执行异常
*/
bool EnumProcess(IN pfTypeEnumProcess fpEnum, IN void* callBackParameter);


//遍历线程函数,返回true结束遍历，返回false继续遍历
typedef bool(*pfTypeEnumThread)(IN  void *callBackParameter, IN THREADENTRY32* pThreadEntry);
/*
	作用：遍历线程
	参数：
	fpEnum ：遍历线程使用的回调函数
	callBackParameter：遍历进程传递个回调函数的参数
	返回值：如果返回true代表正常返回，否则执行异常
*/
bool EnumThread(IN pfTypeEnumThread pfEnum, IN void* callBackParameter);



//遍历模块函数,返回true结束遍历，返回false继续遍历
typedef bool(*pfTypeEnumModule)(IN  void *callBackParameter, IN MODULEENTRY32* pModuleEntry);
/*
	作用:根据进程id遍历进程的模块信息
	参数：
	fpEnum ：遍历模块使用的回调函数
	callBackParameter：遍历进程传递个回调函数的参数
	返回值：如果返回true代表正常返回，否则执行异常
*/
bool EnumModule(IN DWORD dwPID, IN pfTypeEnumModule pfEnum, IN void* callBackParameter);