#include <locale.h>
#include "ProcessOperation.h"
#include <tchar.h>

bool EnumModuleFunc(void *callBackParameter, MODULEENTRY32* pModuleEntry)
{
	_tprintf(_T("\t\thModule:%x, szModule:%s\n"), pModuleEntry->modBaseAddr, pModuleEntry->szModule);
	return false;
}

bool EnumProcessFunc(void *callBackParameter, PROCESSENTRY32* pProcessEntry)
{
	_tprintf(_T("进程id:%d, 进程名:%s\n"), pProcessEntry->th32ProcessID, pProcessEntry->szExeFile);
	//枚举模块信息
	EnumModule(pProcessEntry->th32ProcessID, EnumModuleFunc, NULL);
	return false;
}

bool EnumThreadFunc(void *callBackParameter, THREADENTRY32* pThreadEntry)
{
	_tprintf(_T("所属进程id:%d, 线程id:%d\n"), pThreadEntry->th32OwnerProcessID, pThreadEntry->th32ThreadID);
	return false;
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "chs");

	//遍历进程
	EnumProcess(EnumProcessFunc, NULL);

	//遍历线程
	EnumThread(EnumThreadFunc, NULL);

	return 0;
}