#include <windows.h>
#include <stdio.h>

LONG CALLBACK VehHandler(EXCEPTION_POINTERS* pException) {
	printf("VehHandler函数被调用\n");
	return EXCEPTION_CONTINUE_SEARCH;   //继续投递给seh  0
}

LONG CALLBACK VchHandler(
	_In_  PEXCEPTION_POINTERS pExceptionInfo
	)
{
	printf("VCH函数被调用\n");
	DWORD dwOldProtect;
	VirtualProtect(pExceptionInfo->ExceptionRecord->ExceptionAddress, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	PBYTE pByte = (PBYTE)pExceptionInfo->ExceptionRecord->ExceptionAddress;
	memset(pByte, 0x90, 10);
	VirtualProtect(pExceptionInfo->ExceptionRecord->ExceptionAddress, 1, dwOldProtect, &dwOldProtect);
//	return EXCEPTION_CONTINUE_EXECUTION;  //不会投递给seh  -1

	return EXCEPTION_CONTINUE_EXECUTION;
}


EXCEPTION_DISPOSITION  NTAPI SehHandle(
	_EXCEPTION_RECORD* pExceptionRecord,
	PVOID EstablishFrame,
	CONTEXT *ContextRecord,
	PVOID DispatcherContext)
{
	printf("SEH 被调用\n");

	return ExceptionContinueSearch;
}

typedef struct _MYTEB {
	EXCEPTION_REGISTRATION_RECORD* pExcetionHeader;
}MYTEB, *PMYTEB;


int main()
{
	AddVectoredExceptionHandler(TRUE, &VehHandler);

	AddVectoredContinueHandler(TRUE, &VchHandler);

	PMYTEB pMyTeb = (PMYTEB)NtCurrentTeb();

	EXCEPTION_REGISTRATION_RECORD sehNode = { 0 };
	sehNode.Next = pMyTeb->pExcetionHeader;
	sehNode.Handler = SehHandle;
	//
	pMyTeb->pExcetionHeader = &sehNode;

	//

	*(int*)0 = 0;

	//摘除添加的seh
	pMyTeb->pExcetionHeader = pMyTeb->pExcetionHeader->Next;


	system("pause");
	return 0;
}