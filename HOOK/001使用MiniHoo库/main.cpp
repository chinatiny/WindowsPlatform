#include <Windows.h>


#include "MinHook/include/MinHook.h"

#if defined _M_X64
#pragma comment(lib, "MinHook/bin/MinHook.x64.lib")
#elif defined _M_IX86
#pragma comment(lib, "MinHook/bin/MinHook.x86.lib")
#endif

// Helper function for MH_CreateHookApi().
template <typename T>
inline MH_STATUS MH_CreateHookApiEx(LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour, T** ppOriginal)
{
	return MH_CreateHookApi(pszModule, pszProcName, pDetour, reinterpret_cast<LPVOID*>(ppOriginal));
}

typedef int (WINAPI *MESSAGEBOXW)(HWND, LPCWSTR, LPCWSTR, UINT);

// Pointer for calling original MessageBoxW.
MESSAGEBOXW fpMessageBoxW = NULL;

// Detour function which overrides MessageBoxW.
int WINAPI DetourMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
	return fpMessageBoxW(hWnd, L"Hooked!", lpCaption, uType);
}

int main()
{
	// Initialize MinHook.
	// 1. 初始化MinHOOK库
	if (MH_Initialize() != MH_OK)
	{
		return 1;
	}

	// Create a hook for MessageBoxW, in disabled state.
	if (MH_CreateHookApiEx(L"user32", "MessageBoxW", &DetourMessageBoxW, &fpMessageBoxW) != MH_OK)
	{
		return 1;
	}

	// Enable the hook for MessageBoxW.
	// 启用HOOK
	if (MH_EnableHook(&MessageBoxW) != MH_OK)
	{
		return 1;
	}

	// Expected to tell "Hooked!".
	MessageBoxW(NULL, L"Not hooked...", L"MinHook Sample", MB_OK);

	// Disable the hook for MessageBoxW.
	// 禁用HOOK
	if (MH_DisableHook(&MessageBoxW) != MH_OK)
	{
		return 1;
	}

	// Expected to tell "Not hooked...".
	MessageBoxW(NULL, L"Not hooked...", L"MinHook Sample", MB_OK);

	// Uninitialize MinHook.
	if (MH_Uninitialize() != MH_OK)
	{
		return 1;
	}

	return 0;
}
