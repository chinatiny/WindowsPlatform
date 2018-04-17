#include "ProcessOperation.h"

//遍历进程
bool EnumProcess(IN pfTypeEnumProcess fpEnum, IN void* callBackParameter)
{
	HANDLE hProcessSnap;
	PROCESSENTRY32  stcPe32 = { 0 };
	stcPe32.dwSize = sizeof(PROCESSENTRY32);

	//1. 创建一个进程快照的句柄
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hProcessSnap) return false;

	//2. 通过快照句柄获取第一个进程的信息
	if (!Process32First(hProcessSnap, &stcPe32))
	{
		CloseHandle(hProcessSnap);
		return false;
	}
	//投递第一个回调
	if (fpEnum(callBackParameter, &stcPe32)) goto __END;

	//3. 循环遍历进程信息
	do 
	{
		if (fpEnum(callBackParameter, &stcPe32)) goto __END;
	} while (Process32Next(hProcessSnap, &stcPe32));
	
__END:
	//4.关闭句柄
	CloseHandle(hProcessSnap);

	return true;
}


//遍历线程
bool EnumThread(IN pfTypeEnumThread pfEnum, IN void* callBackParameter)
{
	HANDLE hThreadSnap;
	THREADENTRY32 stcPe32 = { 0 };
	stcPe32.dwSize = sizeof(THREADENTRY32);

	//1. 创建一个进程快照的句柄
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (INVALID_HANDLE_VALUE == hThreadSnap) return false;

	//2. 通过快照句柄获取第一个进程的信息
	if (!Thread32First(hThreadSnap, &stcPe32))
	{
		CloseHandle(hThreadSnap);
		return false;
	}
	//投递第一个回调
	if (pfEnum(callBackParameter, &stcPe32)) goto __END;

	//3. 循环遍历进程信息
	do
	{
		if (pfEnum(callBackParameter, &stcPe32))  goto __END;
	} while (Thread32Next(hThreadSnap, &stcPe32));

__END:
	//4.关闭句柄
	CloseHandle(hThreadSnap);

	return true;
}


bool EnumModule(IN DWORD dwPID, IN pfTypeEnumModule pfEnum, IN void* callBackParameter)
{
	HANDLE        hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32 = { sizeof(MODULEENTRY32) };
	// 1. 创建一个模块相关的快照句柄
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
		return false;
	// 2. 通过模块快照句柄获取第一个模块信息
	if (!Module32First(hModuleSnap, &me32)) {
		CloseHandle(hModuleSnap);
		return false;
	}

	// 3. 循环获取模块信息
	do {
		if (pfEnum(callBackParameter, &me32)) goto __END;
	} while (Module32Next(hModuleSnap, &me32));

__END:
	// 4. 关闭句柄并退出函数
	CloseHandle(hModuleSnap);
	return false;

}