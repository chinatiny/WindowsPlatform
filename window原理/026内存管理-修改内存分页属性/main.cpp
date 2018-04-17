// 内存管理_修改内存分页属性.cpp : 定义控制台应用程序的入口点。
//
#include <Windows.h>
#include <tchar.h>


DWORD handleException(void *pBuff, DWORD dwOldProtect) {

	// 将内存属性修改回原来的属性
	VirtualProtect(pBuff, 10 * sizeof(DWORD), dwOldProtect, &dwOldProtect);

	// 返回EXCEPTION_CONTINUE_EXECUTION时, 会在异常发生点再次执行代码.
	return EXCEPTION_CONTINUE_EXECUTION;
}

int _tmain(int argc, _TCHAR* argv[])
{
	// 1. 得到进程句柄
	HANDLE hDict = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION,
		FALSE,
		1452);
	// 2. 读取进程内存
	TCHAR buff[512] = { 0 };
	DWORD dwRead;
	ReadProcessMemory(hDict,
		(LPVOID)0x5AE6538,
		buff,
		20,
		&dwRead);
	wprintf(L"%s", buff);

	//3. 写进程内存
	WriteProcessMemory(hDict,
		(LPVOID)0x5AE6538,
		L"哈哈哈",
		7,
		&dwRead);
	LPBYTE pDictBuff;
	pDictBuff = (LPBYTE)VirtualAllocEx(hDict,
		NULL,
		64 * 1024,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE);

	// 释放远程进程内存.
	VirtualFreeEx(hDict, pDictBuff, 64 * 1024, MEM_DECOMMIT);

	// 直接读写其它进程内存地址时, 实际上修改的是本进程的
	// 虚拟内存地址.
	//*pDictBuff = 10;
	//VirtualProtectEx(hDict,
	//				(LPVOID)0x5AE6538,
	//				 10,
	//				 PAGE_READONLY,
	//				 &dwRead);


	// 修改内存分页

	DWORD* pBuff = new DWORD;
	*pBuff = 0x12345678;

	DWORD dwOldProtect = 0;
	// 将当前执行代码的内存分页属性修改为只读
	VirtualProtect(pBuff, /*需要和内存分页粒度对齐.如果不对齐,函数会自动向下取整*/
		sizeof(DWORD), /*需要和内存分页粒度对齐,如果不对齐,会自动向上取证*/
		PAGE_READONLY,
		&dwOldProtect /*将旧的内存分页属性输出*/
		);

	HANDLE hProc = OpenProcess(PROCESS_VM_OPERATION,
		FALSE,
		512);

	VirtualProtectEx(hProc,/*要修改的目标进程句柄*/
		(LPVOID)0x1000,/*目标进程中的虚拟地址*/
		sizeof(DWORD), /*需要和内存分页粒度对齐,如果不对齐,会自动向上取证*/
		PAGE_READONLY,
		&dwOldProtect /*将旧的内存分页属性输出*/
		);

	//__try {

	// 尝试修改内存, 但这次会失败,因为没有写的权限
	*pBuff = 0x87654321;

	//}
	//__except( handleException(pBuff,dwOldProtect) ) {
	//}


	return 0;
}

