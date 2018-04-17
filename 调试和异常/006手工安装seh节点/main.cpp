#include <windows.h>
#include <stdio.h>

EXCEPTION_DISPOSITION  NTAPI SehHandle(
	_EXCEPTION_RECORD* pExceptionRecord,
	PVOID EstablishFrame,
	CONTEXT *ContextRecord,
	PVOID DispatcherContext)
{
	printf("sehHandler\n");

	DWORD dwOldProtect;
	VirtualProtect(pExceptionRecord->ExceptionAddress, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	PBYTE pByte = (PBYTE)pExceptionRecord->ExceptionAddress;
	memset(pByte, 0x90, 10);
	VirtualProtect(pExceptionRecord->ExceptionAddress, 1, dwOldProtect, &dwOldProtect);

	return ExceptionContinueExecution;
}

typedef struct _MYTEB {
	EXCEPTION_REGISTRATION_RECORD* pExcetionHeader;
}MYTEB, *PMYTEB;

int main(int argc, char* argv[])
{
	PMYTEB pMyTeb = (PMYTEB)NtCurrentTeb();

	EXCEPTION_REGISTRATION_RECORD sehNode = { 0 };
	sehNode.Next = pMyTeb->pExcetionHeader;
	sehNode.Handler = SehHandle;
	//
	pMyTeb->pExcetionHeader = &sehNode;

	*(int*)0 = 0;

	printf("0指针问题已经被处理掉了\n");

	//摘除添加的seh
	pMyTeb->pExcetionHeader = pMyTeb->pExcetionHeader->Next;

	return 0;
}