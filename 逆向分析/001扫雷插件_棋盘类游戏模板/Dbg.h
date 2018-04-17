#pragma  once
#include <windows.h>

//创建一个调试用的控制台
void CreateDbgConsole();
//格式化输出信息字符串到输出缓冲区 =>利用这个函数可以把程序的日志信息定位到文件中去
void SprintfDbg(TCHAR *pOutBuff, int nMaxOutBuffSize, LPCSTR lpszModuleName, int nLine, LPCTSTR lptstrFormat, va_list argList);

//打印出调试信息
void PrintDbg(LPCSTR lpszModuleName, int nLine, LPCTSTR lptstrFormat, ...);

//打印出窗体消息信息
void PrintWindowsMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LPCSTR lpszModuleName = __FILE__, int nLine = __LINE__);


#ifdef _DEBUG

#define  PrintDbgInfo(lptstrFormat, ...)    do{PrintDbg(__FILE__, __LINE__, lptstrFormat, __VA_ARGS__);}while(0);
#define  PrintWindowsMsgInfo(hwnd, uMsg, wParam, lParam) do{ PrintWindowsMsg(hwnd, uMsg, wParam, lParam, __FILE__, __LINE__);}while (0);

#else

#define  PrintDbgInfo(lptstrFormat, ...) 
#define  PrintWindowsMsgInfo(hwnd, uMsg, wParam, lParam)

#endif

