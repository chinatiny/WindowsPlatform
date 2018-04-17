#include <windows.h>
#include <stdio.h>

typedef NTSTATUS (__stdcall *fpTypeNtReadVirtualMemory)(
	HANDLE ProcessHandle, 
	PVOID BaseAddress, 
	PVOID Buffer, 
	SIZE_T NumberOfBytesToRead, 
	PSIZE_T NumberOfBytesRead);

__declspec(naked) void MyNtReadVirtualMemory()
{
	__asm
	{
		mov     eax, 3Ch;
		xor     ecx, ecx;
		lea     edx, [esp + 4];
		call fs : [0xC0];
		add     esp, 0x4;
		retn    0x14;
	}
}

int main(int argc, char* argv[])
{
	char buff[100] = { 0 };
	char szMsg[] = "hello world";
	fpTypeNtReadVirtualMemory fpMyReadProcessMemory = (fpTypeNtReadVirtualMemory)MyNtReadVirtualMemory;
	DWORD dwRealRead;
	fpMyReadProcessMemory(GetCurrentProcess(), szMsg, buff, strlen(szMsg), &dwRealRead);
	printf("real write:%d\n", dwRealRead);
	return 0;
}