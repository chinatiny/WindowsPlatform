#include <windows.h>
typedef enum THREAD_INFO_CLASS{
	ThreadHideFromDebugger = 17
};
typedef NTSTATUS(NTAPI *ZW_SET_INFORMATION_THREAD)(
	IN  HANDLE 			ThreadHandle,
	IN  THREAD_INFO_CLASS	ThreadInformaitonClass,
	IN  PVOID 			ThreadInformation,
	IN  ULONG 			ThreadInformationLength);

void ZSIT_DetachDebug()
{
	ZW_SET_INFORMATION_THREAD Func;
	Func = (ZW_SET_INFORMATION_THREAD)GetProcAddress(
		LoadLibrary(L"ntdll.dll"), "ZwSetInformationThread");
	Func(GetCurrentThread(), ThreadHideFromDebugger, NULL, NULL);
}


int main(int argc, char* argv[])
{
	ZSIT_DetachDebug();
	MessageBoxA(0, "", "", 0);
	return 0;
}