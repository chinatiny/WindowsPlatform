#include <windows.h>
#include <stdio.h>

void test() {
	MessageBox(0, L"test", 0, 0);
}

int * g_pNum = nullptr;

int exceptHandler(EXCEPTION_POINTERS* pException) {
	printf("异常代码:%08X\n",
		pException->ExceptionRecord->ExceptionCode);
	printf("产生异常的地址:%08X\n",
		pException->ExceptionRecord->ExceptionAddress);
	//pException->ContextRecord->Eip = (DWORD)&test;
	printf("异常过滤函数被调用\n");

	// g_pNum = new int(0);
	pException->ContextRecord->Eax = (DWORD)new int(0);

	return EXCEPTION_CONTINUE_EXECUTION;
}

void fun() {
	__try {
		printf("try块\n");
		*g_pNum = 0;
	}
	__except (exceptHandler(GetExceptionInformation())) {
		printf("except块被调用\n");
	}
}


int main()
{
	__try {
		fun();
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		printf("main::except被调用\n");
	}



	return 0;
}
