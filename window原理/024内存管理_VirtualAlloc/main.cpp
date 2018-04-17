#include <stdio.h>
#include <windows.h>

int main(int argc, char* argv[])
{
	LPVOID pBuff = NULL;
	LPVOID pReserveBuff = NULL;
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	//内存的分配粒度为64k
	pReserveBuff = VirtualAlloc(
		NULL,
		1024 * 64 * 3,
		MEM_RESERVE,
		PAGE_NOACCESS
		);

	if (NULL == pBuff)
	{
		printf("预定义内存失败\n");
	}

	// 尝试写入刚刚预定的内存,
	__try {
		*(DWORD*)pBuff = 0;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		printf("预定的内存必须经过提交才能使用\n");
	}

	//内存的分页粒度为4k，和内存分配粒度不大一样哦！！！
	pBuff = VirtualAlloc(pReserveBuff,
		4096, 
		MEM_COMMIT,
		PAGE_READWRITE
		);

	pBuff = VirtualAlloc((LPVOID)((DWORD)pBuff + 4096), 
		4096,  
		MEM_COMMIT,
		PAGE_READWRITE
		);

	VirtualAlloc((LPVOID)((DWORD)pBuff + 4096),
		4090,
		MEM_COMMIT,
		PAGE_READWRITE
		);

	ZeroMemory(&mbi, sizeof(mbi));

	//内存状态：
	// #define MEM_COMMIT                  0x1000
	// #define MEM_RESERVE                 0x2000
	// #define MEM_DECOMMIT                0x4000
	// #define MEM_RELEASE                 0x8000

	//内存类型：
	// #define MEM_FREE                    0x10000
	// #define MEM_PRIVATE                 0x20000
	// #define MEM_MAPPED                  0x40000
	// #define MEM_RESET                   0x80000

	//内存属性
	// #define PAGE_NOACCESS          0x01     
	// #define PAGE_READONLY          0x02     
	// #define PAGE_READWRITE         0x04     
	// #define PAGE_WRITECOPY         0x08   

	// 	typedef struct _MEMORY_BASIC_INFORMATION {
	// 		PVOID BaseAddress;        =>VirtualQuery第一个参数所在页的首地址4k对齐
	// 		PVOID AllocationBase;     =>预定首地址，地址分配粒度为64k
	// 		DWORD AllocationProtect; =>预定时候给的内存属性
	// 		SIZE_T RegionSize;    => 从BaseAddress开始，有相同属性（读/写/执行）的内存
	// 		DWORD State;   =>MEM_RESERVE、MEM_DECOMMIT、MEM_RELEASE、MEM_COMMIT
	// 		DWORD Protect;  =>提交时候给的内存属性
	// 		DWORD Type;  => MEM_PRIVATE、MEM_MAPPED、MEM_RESET
	// 	} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;
	

	//
	VirtualQuery((LPVOID)((DWORD)pBuff  + 24),
		&mbi,
		sizeof(mbi)
		);

	VirtualFree(pBuff,  /*开始地址*/
		4096,   /*取消提交的字节数*/
		MEM_DECOMMIT  /*取消提交的标志*/
		);

	*(DWORD*)pBuff = 0;

	//
	VirtualFree(pReserveBuff,
		0,
		MEM_RELEASE);

	//测试报错
	//*(DWORD*)pBuff = 0;

	system("pause");
	return 0;
}