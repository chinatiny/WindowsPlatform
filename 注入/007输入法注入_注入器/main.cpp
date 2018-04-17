#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <Imm.h>
#pragma comment(lib,"IMM32.lib")


typedef struct
{
	HWND hWnd;
	DWORD dwPid;
}WNDINFO;

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	WNDINFO* pInfo = (WNDINFO*)lParam;
	DWORD dwProcessId = 0;
	GetWindowThreadProcessId(hWnd, &dwProcessId);

	if (dwProcessId == pInfo->dwPid)
	{
		pInfo->hWnd = hWnd;
		return FALSE;
	}
	return TRUE;
}

HWND GetHwndByProcessId(DWORD dwProcessId)
{
	WNDINFO info = { 0 };
	info.hWnd = NULL;
	info.dwPid = dwProcessId;
	EnumWindows(EnumWindowsProc, (LPARAM)&info);
	return info.hWnd;
}

int main(int argc, char* argv[])
{
	TCHAR ImeDllPath[MAX_PATH] = _T("F:/15pb练习/Windows平台/Debug/007输入法注入_输入法.dll");

	//原始输入法
	HKL HklOldInput = NULL;
	//新的输入法
	HKL HklNewInput = NULL;
	//输入法布局名称
	TCHAR ImeSymbol[MAX_PATH];
	//被注入进程的窗口句柄
	HWND hWnd = NULL;
	HWND hTopWnd = NULL;
	//需要注入的PID
	DWORD dwPID = 0;
	BOOL bRet = FALSE;

	printf("输入要注入到的进程PID:");
	scanf_s("%d[*]", &dwPID);


	HKEY phkResult;
	int i = 0;
	TCHAR ValueName[MAX_PATH];  //存储得到的键值名
	TCHAR lpData[MAX_PATH];     //存储得到的键值数据
	DWORD lenValue = MAX_PATH;  //存储键值的数据长度
	DWORD lenData = MAX_PATH;   //存储键值的数据长度

	//1.保存原始的键盘布局，方便后面还原
	SystemParametersInfo(SPI_GETDEFAULTINPUTLANG, 0, &HklOldInput, 0);

	//2.将输入法DLL和待注入Dll一起放到C:\\WINDOWS\\SYSTEM32目录下
	bRet = CopyFile(ImeDllPath, _T("C:/Windows/System32/MyImeDll.ime"), FALSE);

	//3.加载输入法
	HklNewInput = ImmInstallIME(_T("C:/Windows/System32/MyImeDll.ime"), _T("我的输入法"));

	//4.判断输入法句柄是否有效
	if (!ImmIsIME(HklNewInput))
	{
		MessageBox(NULL, _T("输入法加载失败!!!"), _T("提示"), MB_OK);
		return -1;
	}

	//5.激活新输入法的键盘布局
	ActivateKeyboardLayout(HklNewInput, 0);

	//6.获取被激活的输入法注册表项，方便后面删除
	GetKeyboardLayoutName(ImeSymbol);


	//7.获取窗口句柄
	hWnd = GetHwndByProcessId(dwPID);

	//8.激活指定窗口的输入法
	if (HklNewInput != NULL)
	{
		::PostMessage(hWnd, WM_INPUTLANGCHANGEREQUEST, 0x1, (LPARAM)HklNewInput);
	}

	system("pause");

	//9.还原键盘布局
	SystemParametersInfo(SPI_SETDEFAULTINPUTLANG, 0, &HklOldInput, SPIF_SENDWININICHANGE);

	//10.顶层窗口换回去，不换回去后面卸载不到，因为ime文件占用
	do
	{
		hTopWnd = ::FindWindowEx(NULL, hTopWnd, NULL, NULL);
		if (hTopWnd != NULL)
		{
			::PostMessage(hTopWnd, WM_INPUTLANGCHANGEREQUEST, 0x1, (LPARAM)&HklOldInput);
		}
	} while (hTopWnd == NULL);

	//11.卸载输入法
	if (HklNewInput != NULL)
	{
		UnloadKeyboardLayout(HklNewInput);
	}



	//12.删除注册表项，清理痕迹
	//打开注册表项目，获取句柄
	RegOpenKey(HKEY_CURRENT_USER, L"Keyboard Layout\\Preload", &phkResult);
	//枚举注册表项目
	while (RegEnumValue(phkResult, i, ValueName, &lenValue, NULL, NULL, (LPBYTE)lpData, &lenData) != ERROR_NO_MORE_ITEMS)
	{
		if (lenData != 0)
		{
			if (_tcscmp(ImeSymbol, lpData) == 0)
			{
				//删除项目数值
				RegDeleteValue(phkResult, ValueName);
				break;
			}
		}

		memset(lpData, 0, MAX_PATH);
		memset(ValueName, 0, MAX_PATH);
		lenValue = MAX_PATH;
		lenData = MAX_PATH;
		i++;
	}

	// 删除输入法文件
	DeleteFile(_T("C:/Windows/System32/MyImeDll.ime"));

	return 0;
}