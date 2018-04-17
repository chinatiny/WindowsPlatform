#include "AttachAnti.h"
#include <windows.h>
#include <tchar.h>


typedef void(__stdcall *fpTypeDbgBreakPoint)();

fpTypeDbgBreakPoint s_fpDbgBreakPoint = NULL;


void AntiAttach()
{
	HMODULE hNtDll = LoadLibrary(_T("ntdll.dll"));
	s_fpDbgBreakPoint = (fpTypeDbgBreakPoint)GetProcAddress(hNtDll, "DbgBreakPoint");

	BYTE shellCode[] = {0xC3};
	DWORD dwOldProtected;
	VirtualProtect(s_fpDbgBreakPoint, 1, PAGE_EXECUTE_READWRITE, &dwOldProtected);
	memcpy(s_fpDbgBreakPoint, shellCode, sizeof(shellCode));
	VirtualProtect(s_fpDbgBreakPoint, 1, dwOldProtected, &dwOldProtected);
}
