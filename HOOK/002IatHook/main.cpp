#include <windows.h>

bool HookIat(
	const char *pszDllName,
	const char* pszFunction,
	LPVOID pNewFunction
	);

// 盗版的MessageBox
DWORD WINAPI MyMessageBox(HWND hWnd, TCHAR* pText, TCHAR* pTitle, DWORD type) 
{

	// 还原IAT
	HookIat("User32.dll",
		"MessageBoxW",
		GetProcAddress(GetModuleHandleA("User32.dll"), 
		"MessageBoxW")
		);

	// 调用原版函数
	MessageBox(0, L"在盗版的MessageBox中弹出此框", L"提示", 0);


	// HOOK IAT
	HookIat("User32.dll", "MessageBoxW", &MyMessageBox);

	return 0;
}

int main(int argc, char* argv[])
{

	MessageBox(0, L"正版API", L"提示", 0);
	// HOOK IAT
	HookIat("User32.dll", "MessageBoxW", &MyMessageBox);

	MessageBox(0, L"正版API", L"提示", 0);

	MessageBox(0, L"正版API", L"提示", 0);

	return 0;
}

bool HookIat(
	const char *pszDllName,
	const char* pszFunction,
	LPVOID pNewFunction
	)
{

	PIMAGE_DOS_HEADER			pDosHeader; // Dos头
	PIMAGE_NT_HEADERS			pNtHeader;	// Nt头
	PIMAGE_IMPORT_DESCRIPTOR	pImpTable;	// 导入表
	PIMAGE_THUNK_DATA			pInt;		// 导入表中的导入名称表
	PIMAGE_THUNK_DATA			pIat;		// 导入表中的导入地址表
	DWORD						hModule;
	char*						pFunctionName;
	DWORD						dwOldProtect;

	hModule = (DWORD)GetModuleHandle(NULL);
	// 读取dos头
	pDosHeader = (PIMAGE_DOS_HEADER)hModule;
	// 读取Nt头
	pNtHeader = (PIMAGE_NT_HEADERS)(hModule + pDosHeader->e_lfanew);
	//获取导入表
	pImpTable = (PIMAGE_IMPORT_DESCRIPTOR)
		(hModule + pNtHeader->OptionalHeader.DataDirectory[1].VirtualAddress);

	while (pImpTable->FirstThunk != 0 && pImpTable->OriginalFirstThunk != 0)
	{
		// 判断是否找到了对应的模块名
		if (_stricmp((char*)(pImpTable->Name + hModule), pszDllName) != 0)
		{
			++pImpTable;
			continue;
		}

		//遍历名称表找到函数名
		// 遍历名称表,找到函数名
		pInt = (PIMAGE_THUNK_DATA)(pImpTable->OriginalFirstThunk + hModule);
		pIat = (PIMAGE_THUNK_DATA)(pImpTable->FirstThunk + hModule);

		while (pInt->u1.AddressOfData != 0) {

			// 判断是以名称导入还是以需要导入
			if ((pInt->u1.Ordinal & 0x80000000) == 1)
			{
				// 以序号导入

				// 判断是否找到了对应的函数序号
				if (pInt->u1.Ordinal == (((DWORD)pszFunction) & 0xFFFF)) 
				{
					// 找到之后,将钩子函数的地址写入到iat
					VirtualProtect(&pIat->u1.Function,
						4,
						PAGE_READWRITE,
						&dwOldProtect
						);

					pIat->u1.Function = (DWORD)pNewFunction;

					VirtualProtect(&pIat->u1.Function,
						4,
						dwOldProtect,
						&dwOldProtect
						);
					return true;
				}
			}
			else
			{
				// 以名称导入
				pFunctionName = (char*)(pInt->u1.Function + hModule + 2);

				// 判断是否找到了对应的函数名
				if (strcmp(pszFunction, pFunctionName) == 0) 
				{
					VirtualProtect(&pIat->u1.Function,
						4,
						PAGE_READWRITE,
						&dwOldProtect
						);

					// 找到之后,将钩子函数的地址写入到iat
					pIat->u1.Function = (DWORD)pNewFunction;

					VirtualProtect(&pIat->u1.Function,
						4,
						dwOldProtect,
						&dwOldProtect
						);

					return true;
				}
			}

			++pIat;
			++pInt;
		}
	}

	return true;
}