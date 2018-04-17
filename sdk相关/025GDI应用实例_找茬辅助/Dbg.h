#pragma  once
#include <windows.h>


//格式化输出信息字符串到输出缓冲区 =>利用这个函数可以把程序的日志信息定位到文件中去
void SprintfDbgInfo(TCHAR *pOutBuff, int nMaxOutBuffSize, LPCTSTR lptstrFormat, va_list argList);


//打印出调试信息
void PrintDbgInfo(LPCTSTR lptstrFormat, ...);

//打印出窗体消息信息
void PrintWindowsMsgInfo(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
