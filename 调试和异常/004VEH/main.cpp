#include <windows.h>
#include <stdio.h>

LONG CALLBACK VehHandler(EXCEPTION_POINTERS* pException) {
	printf("VehHandler函数被调用\n");
	DWORD dwOldProtect;
	VirtualProtect(pException->ExceptionRecord->ExceptionAddress, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	PBYTE pByte = (PBYTE)pException->ExceptionRecord->ExceptionAddress;
	memset(pByte, 0x90, 10);
	VirtualProtect(pException->ExceptionRecord->ExceptionAddress, 1, dwOldProtect, &dwOldProtect);
	//return EXCEPTION_CONTINUE_EXECUTION;  //不会投递给seh  -1
	return EXCEPTION_CONTINUE_SEARCH;   //继续投递给seh  0
}

int main()
{
	AddVectoredExceptionHandler(TRUE,
		&VehHandler);

	__try {
		*(int*)0 = 0;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		printf("main::except被调用\n");
	}
	system("pause");
	return 0;
}