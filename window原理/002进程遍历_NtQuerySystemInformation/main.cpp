#include <Windows.h>
#include "UnDocoumentApi.h"
#include <stdio.h>
#include <locale.h>

int main(int argc, char* argv[])
{
	// 设置本地字符集
	setlocale(LC_ALL, "chs");

	//////////////////////////////////////////////////////////////////////////
	// 利用未文档化的API遍历进程信息
	// 由于NtQuerySystemInformation函数没有文档化, 因此,windwos的头文件中没有该函数的
	// 声明, 因此, 想要使用这个函数, 必须自己获取.
	// 获取方法:
	// 1. 该函数是ntdll.dll的导出函数, 因此,可以使用LoadLibrary得到ntdll.dll的加载基址
	//    再通过GetProcAddress函数获取该函数的地址.
	// 2. 定义函数指针, 接收该函数的地址.
	// 3. 调用该函数
	// 4. 遍历进程信息
	//

	//1. 得到函数地址
	HMODULE hNtdll = LoadLibrary(L"ntdll.dll");
	fnNtQuerySystemInformation ZwQuerySystemInformation;

	ZwQuerySystemInformation = \
		(fnNtQuerySystemInformation)GetProcAddress(hNtdll, "NtQuerySystemInformation");


	//2. 调用函数, 这次调用,是为了得到保存信息的缓冲区字节数
	DWORD dwNeedSize = 0;
	ZwQuerySystemInformation(SystemProcessesAndThreadsInformation, /*要查询的信息的类型*/
		0, /*保存信息的缓冲区首地址*/
		0, /*保存信息的缓冲区的最大字节数*/
		&dwNeedSize/*函数输出的信息的最大字节数*/
		);

	//2.1 根据得到的缓冲区大小申请堆空间
	SYSTEM_PROCESS_INFORMATION* pProcInfo = NULL;
	pProcInfo = (SYSTEM_PROCESS_INFORMATION *)new BYTE[dwNeedSize];

	//2.2 再次调用函数,获取进程信息
	ZwQuerySystemInformation(SystemProcessesAndThreadsInformation,  /*要查询的信息的类型*/
		pProcInfo,/*保存信息的缓冲区首地址*/
		dwNeedSize,/*保存信息的缓冲区的最大字节数*/
		&dwNeedSize /*函数输出的信息的最大字节数*/
		);


	//3. 开始遍历
	while (pProcInfo->NextEntryDelta != 0) {

		// 3.1 输出信息
		wprintf(L"PID: %5d Name:%s\n",
			pProcInfo->InheritedFromProcessId,/*进程PID*/
			pProcInfo->ProcessName.Buffer  /*进程名*/
			);


		// 3.2 遍历到下一个结构体
		pProcInfo = (SYSTEM_PROCESS_INFORMATION*)((DWORD)pProcInfo + pProcInfo->NextEntryDelta);
	}

}