#include <windows.h>
#include <stdio.h>

typedef NTSTATUS(__stdcall *fpTypeNtReadVirtualMemory)(
	ULONG uFromPid, 
	PVOID  pFromAddress, 
	PVOID pToBuff,
	ULONG uReadSize);


//64位和32位不一样
__declspec(naked) void MyNtReadVirtualMemory()
{
	__asm
	{
		mov     eax, 0x191;
		mov     edx, 7FFE0300h;
		call    dword ptr[edx];
		retn    0x10;
	}
}

int main(int argc, char* argv[])
{
	char buff[100] = { 0 };
	char szMsg[] = "hello world";
	fpTypeNtReadVirtualMemory fpMyReadProcessMemory = (fpTypeNtReadVirtualMemory)MyNtReadVirtualMemory;
	fpMyReadProcessMemory(GetCurrentProcessId(), szMsg, buff, strlen(szMsg));

	printf("buff:%s\n", buff);
	system("pause");
	return 0;
}