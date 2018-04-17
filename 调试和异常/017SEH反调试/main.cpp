#include <windows.h>
#include <stdio.h>


#pragma warning(disable: 4733) 

void fun()
{
	printf("helloworld");
}
EXCEPTION_DISPOSITION ExpHandel_B(
	// 异常状态描述
	EXCEPTION_RECORD              *pExceptionRecord,
	// 异常注册框架
	EXCEPTION_REGISTRATION_RECORD *pEstablisherFrame,
	// 返回线程上下文
	CONTEXT                       *pContextRecord,
	// 分发器上下文（系统使用，无需关注）
	PVOID                         pDispatcherContext)
{
	pContextRecord->Eip = (DWORD)fun;
	// 返回“运行下一个异常处理器”

	return ExceptionContinueExecution;

}

int main(int argc, char* argv[])
{
	_asm{
		push ExpHandel_B;
		push FS : [0];
		mov FS : [0], esp;
	}
	_asm
	{
		int 3;
	}
	MessageBoxA(0, 0, 0, 0);
	_asm

	{
		mov eax, FS:[0];
		mov eax, [eax];
		mov FS : [0], eax;
		pop eax;
		pop eax;
	}
	return 0;
}

