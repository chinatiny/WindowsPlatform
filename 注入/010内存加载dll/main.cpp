#include <windows.h>
#include <tchar.h>
#include "MemoryModule.h"

DWORD WINAPI TestThreadProc(
	_In_  LPVOID lpParameter
	)
{
	while (true)
	{
		_tprintf(_T("hello thread\n"));
	}

	return 1;
}


typedef HANDLE(__stdcall *fpTypeCreateThread)(
	LPSECURITY_ATTRIBUTES lpThreadAttributes, 
	SIZE_T dwStackSize, 
	LPTHREAD_START_ROUTINE lpStartAddress, 
	LPVOID lpParameter,
	DWORD dwCreationFlags,
	LPDWORD lpThreadId);

int main(int argc, char* argv[])
{

	HANDLE hFile = NULL;
	hFile = CreateFile(_T("F:\\15pb练习\\Windows平台\\注入\\010内存加载dll\\KernelBase.dll"),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return false;
	}
	DWORD	dwLowSize = GetFileSize(hFile, NULL);
	PBYTE pFileBuff = new BYTE[dwLowSize];
	DWORD dwRealRead = 0;
	ReadFile(hFile, pFileBuff, dwLowSize, &dwRealRead, NULL);


	HMEMORYMODULE hMod = MemoryLoadLibrary(pFileBuff, dwLowSize);
	fpTypeCreateThread fpCreateThread = (fpTypeCreateThread)MemoryGetProcAddress(hMod, "CreateThread");

	DWORD dwThreadID = 0;
	fpCreateThread(NULL, 0, TestThreadProc, NULL, NULL, &dwThreadID);


	system("pause");
	delete[]pFileBuff;
	CloseHandle(hFile);

	return 0;
}