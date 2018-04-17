#include <windows.h>
#include <tchar.h>

int main(int argc, char* argv[])
{

	BYTE *pNewBuff = new BYTE[4];
	//情况一:new出的内存在debug和release下规律
	/*
		pNewBuff 在debug下:fd fd fd fd开始和fd fd fd fd结束
		未初始化:fd fd fd fd
		0x0045755C  fd fd fd fd  ????
		0x00457560  cd cd cd cd  ????
		0x00457564  fd fd fd fd  ????

		pNewBuff 在release下:没有任何规律
		0x004D7BAC  2f 77 00 1c  /w..
		0x004D7BB0  0d f0 ad ba  .???
		0x004D7BB4  ab ab ab ab  ????
	*/

	//情况二:delete new出来的内存在debug和release下的规律
	delete [] pNewBuff;
	/*
		pNewBuff在debug的规律
		0x0044755C  ee fe ee fe  ????
		0x00447560  ee fe ee fe  ????
		0x00447564  ee fe ee fe  ????

		pNew在release下,当前地址不是ee fe ee fe,它的下面是ee fe ee fe
		0x007C7BB0  f0 8f 7c 00  ??|.
		0x007C7BB4  c4 00 7c 00  ?.|.
		0x007C7BB8  ee fe ee fe  ????
		0x007C7BBC  ee fe ee fe  ????
		0x007C7BC0  ee fe ee fe  ????
	*/

	//情况三:局部变量
	BYTE byArry[100];
	/*
		byArry 在debug和release下:
		0x0040FB8C  cc cc cc cc  ????
		0x0040FB90  cc cc cc cc  ????
		0x0040FB94  cc cc cc cc  ????
		0x0040FB98  cc cc cc cc  ????
		0x0040FB9C  cc cc cc cc  ????
	*/

	//情况四:VirtualAlloc申请地址
	LPVOID lpVirtualAlloc = VirtualAlloc(NULL, 1000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	/*
		1. lpVirtualAlloc 分配出来的内存以64k对齐
		2. 目前在debug和release下都为0
	*/

	for (int i = 0; i < 1000; i++)
	{
		((BYTE*)lpVirtualAlloc)[i] = 0xFF;
	}

	VirtualFree(lpVirtualAlloc, 1000, MEM_RELEASE | MEM_DECOMMIT);

	//情况五:HeapAlloc
	HANDLE	hHeap = NULL;
	hHeap = HeapCreate(0, 0, 0);

	LPVOID lpHeap = HeapAlloc(hHeap,	
		HEAP_ZERO_MEMORY, 
		100);
	HeapFree(hHeap, 0, lpHeap);

	/*
		1. hHeap 是64k对齐,明显调用了 VirtualAlloc
		2. lpHeap 是对hHeap分配出来的内存地址的管理,地址没有规律
	*/

	//情况六:浮点数据
	float fNum = 12.5;
	_tprintf(_T("fNum addr:%08X\n"), &fNum);

	/*
		浮点数特点:
		1. 高位数大,低位基本为0
	*/


	_tprintf(_T("pNewBuff:%08X\n"), pNewBuff);
	_tprintf(_T("byArry:%08X\n"), byArry);
	system("pause");
	return 0;
}