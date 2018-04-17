#include "Dbg.h"
#include <tchar.h>
#include   <stdarg.h>
#include <Shlwapi.h>
#include <map>
#include <string>
#pragma  comment(lib, "Shlwapi.lib")


// 多字符转换为宽字符(Unicode  --> ASCII)
#define  WCHAR_TO_CHAR(lpW_Char, lpChar) \
	WideCharToMultiByte(CP_ACP, NULL, lpW_Char, -1, lpChar, _countof(lpChar), NULL, FALSE);

// 多字符转换为宽字符(ASCII --> Unicode)
#define  CHAR_TO_WCHAR(lpChar, lpW_Char) \
	MultiByteToWideChar(CP_ACP, NULL, lpChar, -1, lpW_Char, _countof(lpW_Char));

//控制台句柄
HANDLE s_hOutput = NULL;
bool s_bRaw = false;


//创建一个调试用的控制台
void CreateDbgConsole()
{
#ifdef _DEBUG
	if (AllocConsole())
	{
		s_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	}

#endif // DEBUG
}

//格式化输出信息字符串到输出缓冲区
void SprintfDbg(TCHAR *pOutBuff, int nMaxOutBuffSize, LPCSTR lpszModuleName, int nLine, LPCTSTR lptstrFormat, va_list argList)
{
	
	//
	const int nFileBuffSize = 256;
	TCHAR fileNameBuff[nFileBuffSize];

	//设置文件名和行号
#ifdef UNICODE
	CHAR_TO_WCHAR(lpszModuleName, fileNameBuff);
	TCHAR *pFileName = PathFindFileName(fileNameBuff);
	_stprintf_s(pOutBuff, nMaxOutBuffSize - sizeof(TCHAR), TEXT("[%s:%d]  ::  "), pFileName, nLine);
#else
	_tcscpy_s(fileNameBuff, lpszModuleName);
	TCHAR *pFileName = PathFindFileName(fileNameBuff);
	_stprintf_s(pOutBuff, nMaxOutBuffSize - sizeof(TCHAR), TEXT("[%s:%d]  ::"), pFileName, nLine);
#endif

	//设置输入信息
	int nHeadLen = _tcslen(pOutBuff);
	_vstprintf_s(pOutBuff + nHeadLen, nMaxOutBuffSize - sizeof(TCHAR)-nHeadLen * sizeof(TCHAR), lptstrFormat, argList);

	//设置换行
	int nBodyLen = _tcslen(pOutBuff);
	_stprintf_s(pOutBuff + nBodyLen, nMaxOutBuffSize - sizeof(TCHAR)-nBodyLen * sizeof(TCHAR), TEXT("\r\n"));
}

//打印出调试信息
void PrintDbg(LPCSTR lpszModuleName, int nLine, LPCTSTR lptstrFormat, ...)
{
	const int c_nMaxBuffSize = 4096;
	TCHAR outBuff[c_nMaxBuffSize];

	//把结果输出到缓冲区
	va_list argList;
	va_start(argList, lptstrFormat);
	SprintfDbg(outBuff, c_nMaxBuffSize, lpszModuleName, nLine, lptstrFormat, argList);
	va_end(argList);

	if (NULL != s_hOutput)
	{
		WriteConsole(s_hOutput, outBuff, _tcslen(outBuff), NULL, NULL);
	}
	else
	{
		OutputDebugString(outBuff);
	}
}

#ifdef UNICODE
void InitWindowsMsgInfo(std::map<int, std::wstring> &msgMap, bool &bIsInit)
#else
void InitWindowsMsgInfo(std::map<int, std::string> &msgMap, bool &bIsInit)
#endif
{
	bIsInit = true;
	//填充消息结构
	msgMap.insert({ 0x00, TEXT("WM_NULL") });
	msgMap.insert({ 0x01, TEXT("WM_CREATE") });
	msgMap.insert({ 0x02, TEXT("WM_DESTROY") });
	msgMap.insert({ 0x03, TEXT("WM_MOVE") });
	msgMap.insert({ 0x05, TEXT("WM_SIZE") });
	msgMap.insert({ 0x06, TEXT("WM_ACTIVATE") });
	msgMap.insert({ 0x07, TEXT("WM_SETFOCUS") });
	msgMap.insert({ 0x08, TEXT("WM_KILLFOCUS") });
	msgMap.insert({ 0x0A, TEXT("WM_ENABLE") });
	msgMap.insert({ 0x0B, TEXT("WM_SETREDRAW") });
	msgMap.insert({ 0x0C, TEXT("WM_SETTEXT") });
	msgMap.insert({ 0x0D, TEXT("WM_GETTEXT") });
	msgMap.insert({ 0x0E, TEXT("WM_GETTEXTLENGTH") });
	msgMap.insert({ 0x0F, TEXT("WM_PAINT") });
	msgMap.insert({ 0x10, TEXT("WM_CLOSE") });
	msgMap.insert({ 0x11, TEXT("WM_QUERYENDSESSION") });
	msgMap.insert({ 0x12, TEXT("WM_QUIT") });
	msgMap.insert({ 0x13, TEXT("WM_QUERYOPEN") });
	msgMap.insert({ 0x14, TEXT("WM_ERASEBKGND") });
	msgMap.insert({ 0x15, TEXT("WM_SYSCOLORCHANGE") });
	msgMap.insert({ 0x16, TEXT("WM_ENDSESSION") });
	msgMap.insert({ 0x17, TEXT("WM_SYSTEMERROR") });
	msgMap.insert({ 0x18, TEXT("WM_SHOWWINDOW") });
	msgMap.insert({ 0x19, TEXT("WM_CTLCOLOR") });
	msgMap.insert({ 0x1A, TEXT("WM_WININICHANGE  OR WM_SETTINGCHANGE") });
	msgMap.insert({ 0x1B, TEXT("WM_DEVMODECHANGE") });
	msgMap.insert({ 0x1C, TEXT("WM_ACTIVATEAPP") });
	msgMap.insert({ 0x1D, TEXT("WM_FONTCHANGE") });
	msgMap.insert({ 0x1E, TEXT("WM_TIMECHANGE") });
	msgMap.insert({ 0x1F, TEXT("WM_CANCELMODE") });
	msgMap.insert({ 0x20, TEXT("WM_SETCURSOR") });
	msgMap.insert({ 0x21, TEXT("WM_MOUSEACTIVATE") });
	msgMap.insert({ 0x22, TEXT("WM_CHILDACTIVATE") });
	msgMap.insert({ 0x23, TEXT("WM_QUEUESYNC") });
	msgMap.insert({ 0x24, TEXT("WM_GETMINMAXINFO") });
	msgMap.insert({ 0x26, TEXT("WM_PAINTICON") });
	msgMap.insert({ 0x27, TEXT("WM_ICONERASEBKGND") });
	msgMap.insert({ 0x28, TEXT("WM_NEXTDLGCTL") });
	msgMap.insert({ 0x2A, TEXT("WM_SPOOLERSTATUS") });
	msgMap.insert({ 0x2B, TEXT("WM_DRAWITEM") });
	msgMap.insert({ 0x2C, TEXT("WM_MEASUREITEM") });
	msgMap.insert({ 0x2D, TEXT("WM_DELETEITEM") });
	msgMap.insert({ 0x2E, TEXT("WM_VKEYTOITEM") });
	msgMap.insert({ 0x2F, TEXT("WM_CHARTOITEM") });
	msgMap.insert({ 0x30, TEXT("WM_SETFONT") });
	msgMap.insert({ 0x31, TEXT("WM_GETFONT") });
	msgMap.insert({ 0x32, TEXT("WM_SETHOTKEY") });
	msgMap.insert({ 0x33, TEXT("WM_GETHOTKEY") });
	msgMap.insert({ 0x37, TEXT("WM_QUERYDRAGICON") });
	msgMap.insert({ 0x39, TEXT("WM_COMPAREITEM") });
	msgMap.insert({ 0x41, TEXT("WM_COMPACTING") });
	msgMap.insert({ 0x46, TEXT("WM_WINDOWPOSCHANGING") });
	msgMap.insert({ 0x47, TEXT("WM_WINDOWPOSCHANGED") });
	msgMap.insert({ 0x48, TEXT("WM_POWER") });
	msgMap.insert({ 0x4A, TEXT("WM_COPYDATA") });
	msgMap.insert({ 0x4B, TEXT("WM_CANCELJOURNAL") });
	msgMap.insert({ 0x4E, TEXT("WM_NOTIFY") });
	msgMap.insert({ 0x50, TEXT("WM_INPUTLANGCHANGEREQUEST") });
	msgMap.insert({ 0x51, TEXT("WM_INPUTLANGCHANGE") });
	msgMap.insert({ 0x52, TEXT("WM_TCARD") });
	msgMap.insert({ 0x53, TEXT("WM_HELP") });
	msgMap.insert({ 0x54, TEXT("WM_USERCHANGED") });
	msgMap.insert({ 0x55, TEXT("WM_NOTIFYFORMAT") });
	msgMap.insert({ 0x7B, TEXT("WM_CONTEXTMENU") });
	msgMap.insert({ 0x7C, TEXT("WM_STYLECHANGING") });
	msgMap.insert({ 0x7D, TEXT("WM_STYLECHANGED") });
	msgMap.insert({ 0x7E, TEXT("WM_DISPLAYCHANGE") });
	msgMap.insert({ 0x7F, TEXT("WM_GETICON") });
	msgMap.insert({ 0x80, TEXT("WM_SETICON") });
	msgMap.insert({ 0x81, TEXT("WM_NCCREATE") });
	msgMap.insert({ 0x82, TEXT("WM_NCDESTROY") });
	msgMap.insert({ 0x83, TEXT("WM_NCCALCSIZE") });
	msgMap.insert({ 0x84, TEXT("WM_NCHITTEST") });
	msgMap.insert({ 0x85, TEXT("WM_NCPAINT") });
	msgMap.insert({ 0x86, TEXT("WM_NCACTIVATE") });
	msgMap.insert({ 0x87, TEXT("WM_GETDLGCODE") });
	msgMap.insert({ 0xA0, TEXT("WM_NCMOUSEMOVE") });
	msgMap.insert({ 0xA1, TEXT("WM_NCLBUTTONDOWN") });
	msgMap.insert({ 0xA2, TEXT("WM_NCLBUTTONUP") });
	msgMap.insert({ 0xA3, TEXT("WM_NCLBUTTONDBLCLK") });
	msgMap.insert({ 0xA4, TEXT("WM_NCRBUTTONDOWN") });
	msgMap.insert({ 0xA5, TEXT("WM_NCRBUTTONUP") });
	msgMap.insert({ 0xA6, TEXT("WM_NCRBUTTONDBLCLK") });
	msgMap.insert({ 0xA7, TEXT("WM_NCMBUTTONDOWN") });
	msgMap.insert({ 0xA8, TEXT("WM_NCMBUTTONUP") });
	msgMap.insert({ 0xA9, TEXT("WM_NCMBUTTONDBLCLK") });
	msgMap.insert({ 0x100, TEXT("WM_KEYFIRST OR WM_KEYDOWN") });
	msgMap.insert({ 0x101, TEXT("WM_KEYUP") });
	msgMap.insert({ 0x102, TEXT("WM_CHAR") });
	msgMap.insert({ 0x103, TEXT("WM_DEADCHAR") });
	msgMap.insert({ 0x104, TEXT("WM_SYSKEYDOWN") });
	msgMap.insert({ 0x105, TEXT("WM_SYSKEYUP") });
	msgMap.insert({ 0x106, TEXT("WM_SYSCHAR") });
	msgMap.insert({ 0x107, TEXT("WM_SYSDEADCHAR") });
	msgMap.insert({ 0x108, TEXT("WM_KEYLAST") });
	msgMap.insert({ 0x10D, TEXT("WM_IME_STARTCOMPOSITION") });
	msgMap.insert({ 0x10E, TEXT("WM_IME_ENDCOMPOSITION") });
	msgMap.insert({ 0x10F, TEXT("WM_IME_COMPOSITION") });
	msgMap.insert({ 0x10F, TEXT("WM_IME_KEYLAST") });
	msgMap.insert({ 0x110, TEXT("WM_INITDIALOG") });
	msgMap.insert({ 0x111, TEXT("WM_COMMAND") });
	msgMap.insert({ 0x112, TEXT("WM_SYSCOMMAND") });
	msgMap.insert({ 0x113, TEXT("WM_TIMER") });
	msgMap.insert({ 0x114, TEXT("WM_HSCROLL") });
	msgMap.insert({ 0x115, TEXT("WM_VSCROLL") });
	msgMap.insert({ 0x116, TEXT("WM_INITMENU") });
	msgMap.insert({ 0x117, TEXT("WM_INITMENUPOPUP") });
	msgMap.insert({ 0x11F, TEXT("WM_MENUSELECT") });
	msgMap.insert({ 0x120, TEXT("WM_MENUCHAR") });
	msgMap.insert({ 0x121, TEXT("WM_ENTERIDLE") });
	msgMap.insert({ 0x132, TEXT("WM_CTLCOLORMSGBOX") });
	msgMap.insert({ 0x133, TEXT("WM_CTLCOLOREDIT") });
	msgMap.insert({ 0x134, TEXT("WM_CTLCOLORLISTBOX") });
	msgMap.insert({ 0x135, TEXT("WM_CTLCOLORBTN") });
	msgMap.insert({ 0x136, TEXT("WM_CTLCOLORDLG") });
	msgMap.insert({ 0x137, TEXT("WM_CTLCOLORSCROLLBAR") });
	msgMap.insert({ 0x138, TEXT("WM_CTLCOLORSTATIC") });
	msgMap.insert({ 0x200, TEXT("WM_MOUSEFIRST") });
	msgMap.insert({ 0x200, TEXT("WM_MOUSEMOVE") });
	msgMap.insert({ 0x201, TEXT("WM_LBUTTONDOWN") });
	msgMap.insert({ 0x202, TEXT("WM_LBUTTONUP") });
	msgMap.insert({ 0x203, TEXT("WM_LBUTTONDBLCLK") });
	msgMap.insert({ 0x204, TEXT("WM_RBUTTONDOWN") });
	msgMap.insert({ 0x205, TEXT("WM_RBUTTONUP") });
	msgMap.insert({ 0x206, TEXT("WM_RBUTTONDBLCLK") });
	msgMap.insert({ 0x207, TEXT("WM_MBUTTONDOWN") });
	msgMap.insert({ 0x208, TEXT("WM_MBUTTONUP") });
	msgMap.insert({ 0x209, TEXT("WM_MBUTTONDBLCLK") });
	msgMap.insert({ 0x20A, TEXT("WM_MOUSEWHEEL") });
	msgMap.insert({ 0x20E, TEXT("WM_MOUSEHWHEEL") });
	msgMap.insert({ 0x210, TEXT("WM_PARENTNOTIFY") });
	msgMap.insert({ 0x211, TEXT("WM_ENTERMENULOOP") });
	msgMap.insert({ 0x212, TEXT("WM_EXITMENULOOP") });
	msgMap.insert({ 0x213, TEXT("WM_NEXTMENU") });
	msgMap.insert({ 0x214, TEXT("WM_SIZING") });
	msgMap.insert({ 0x215, TEXT("WM_CAPTURECHANGED") });
	msgMap.insert({ 0x216, TEXT("WM_MOVING") });
	msgMap.insert({ 0x218, TEXT("WM_POWERBROADCAST") });
	msgMap.insert({ 0x219, TEXT("WM_DEVICECHANGE") });
	msgMap.insert({ 0x220, TEXT("WM_MDICREATE") });
	msgMap.insert({ 0x221, TEXT("WM_MDIDESTROY") });
	msgMap.insert({ 0x222, TEXT("WM_MDIACTIVATE") });
	msgMap.insert({ 0x223, TEXT("WM_MDIRESTORE") });
	msgMap.insert({ 0x224, TEXT("WM_MDINEXT") });
	msgMap.insert({ 0x225, TEXT("WM_MDIMAXIMIZE") });
	msgMap.insert({ 0x226, TEXT("WM_MDITILE") });
	msgMap.insert({ 0x227, TEXT("WM_MDICASCADE") });
	msgMap.insert({ 0x228, TEXT("WM_MDIICONARRANGE") });
	msgMap.insert({ 0x229, TEXT("WM_MDIGETACTIVE") });
	msgMap.insert({ 0x230, TEXT("WM_MDISETMENU") });
	msgMap.insert({ 0x231, TEXT("WM_ENTERSIZEMOVE") });
	msgMap.insert({ 0x232, TEXT("WM_EXITSIZEMOVE") });
	msgMap.insert({ 0x233, TEXT("WM_DROPFILES") });
	msgMap.insert({ 0x234, TEXT("WM_MDIREFRESHMENU") });
	msgMap.insert({ 0x281, TEXT("WM_IME_SETCONTEXT") });
	msgMap.insert({ 0x282, TEXT("WM_IME_NOTIFY") });
	msgMap.insert({ 0x283, TEXT("WM_IME_CONTROL") });
	msgMap.insert({ 0x284, TEXT("WM_IME_COMPOSITIONFULL") });
	msgMap.insert({ 0x285, TEXT("WM_IME_SELECT") });
	msgMap.insert({ 0x286, TEXT("WM_IME_CHAR") });
	msgMap.insert({ 0x290, TEXT("WM_IME_KEYDOWN") });
	msgMap.insert({ 0x291, TEXT("WM_IME_KEYUP") });
	msgMap.insert({ 0x2A1, TEXT("WM_MOUSEHOVER") });
	msgMap.insert({ 0x2A2, TEXT("WM_NCMOUSELEAVE") });
	msgMap.insert({ 0x2A3, TEXT("WM_MOUSELEAVE") });
	msgMap.insert({ 0x300, TEXT("WM_CUT") });
	msgMap.insert({ 0x301, TEXT("WM_COPY") });
	msgMap.insert({ 0x302, TEXT("WM_PASTE") });
	msgMap.insert({ 0x303, TEXT("WM_CLEAR") });
	msgMap.insert({ 0x304, TEXT("WM_UNDO") });
	msgMap.insert({ 0x305, TEXT("WM_RENDERFORMAT") });
	msgMap.insert({ 0x306, TEXT("WM_RENDERALLFORMATS") });
	msgMap.insert({ 0x307, TEXT("WM_DESTROYCLIPBOARD") });
	msgMap.insert({ 0x308, TEXT("WM_DRAWCLIPBOARD") });
	msgMap.insert({ 0x309, TEXT("WM_PAINTCLIPBOARD") });
	msgMap.insert({ 0x30A, TEXT("WM_VSCROLLCLIPBOARD") });
	msgMap.insert({ 0x30B, TEXT("WM_SIZECLIPBOARD") });
	msgMap.insert({ 0x30C, TEXT("WM_ASKCBFORMATNAME") });
	msgMap.insert({ 0x30D, TEXT("WM_CHANGECBCHAIN") });
	msgMap.insert({ 0x30E, TEXT("WM_HSCROLLCLIPBOARD") });
	msgMap.insert({ 0x30F, TEXT("WM_QUERYNEWPALETTE") });
	msgMap.insert({ 0x310, TEXT("WM_PALETTEISCHANGING") });
	msgMap.insert({ 0x311, TEXT("WM_PALETTECHANGED") });
	msgMap.insert({ 0x312, TEXT("WM_HOTKEY") });
	msgMap.insert({ 0x317, TEXT("WM_PRINT") });
	msgMap.insert({ 0x318, TEXT("WM_PRINTCLIENT") });
	msgMap.insert({ 0x358, TEXT("WM_HANDHELDFIRST") });
	msgMap.insert({ 0x35F, TEXT("WM_HANDHELDLAST") });
	msgMap.insert({ 0x380, TEXT("WM_PENWINFIRST") });
	msgMap.insert({ 0x38F, TEXT("WM_PENWINLAST") });
	msgMap.insert({ 0x390, TEXT("WM_COALESCE_FIRST") });
	msgMap.insert({ 0x39F, TEXT("WM_COALESCE_LAST") });
	msgMap.insert({ 0x3E0, TEXT("WM_DDE_FIRST") });
	msgMap.insert({ 0x3E0, TEXT("WM_DDE_INITIATE") });
	msgMap.insert({ 0x3E1, TEXT("WM_DDE_TERMINATE") });
	msgMap.insert({ 0x3E2, TEXT("WM_DDE_ADVISE") });
	msgMap.insert({ 0x3E3, TEXT("WM_DDE_UNADVISE") });
	msgMap.insert({ 0x3E4, TEXT("WM_DDE_ACK") });
	msgMap.insert({ 0x3E5, TEXT("WM_DDE_DATA") });
	msgMap.insert({ 0x3E6, TEXT("WM_DDE_REQUEST") });
	msgMap.insert({ 0x3E7, TEXT("WM_DDE_POKE") });
	msgMap.insert({ 0x3E8, TEXT("WM_DDE_EXECUTE") });
	msgMap.insert({ 0x3E8, TEXT("WM_DDE_LAST") });
	msgMap.insert({ 0x400, TEXT("WM_USER") });
	msgMap.insert({ 0x8000, TEXT("WM_APP") });
}
void PrintWindowsMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LPCSTR lpszModuleName, int nLine)
{
	static bool bIsInit = false;
#ifdef UNICODE
	static std::map<int, std::wstring> msgMap;
#else
	static std::map<int, std::string> msgMap;
#endif
	if (!bIsInit) InitWindowsMsgInfo(msgMap, bIsInit);

	if (msgMap.find(uMsg) == msgMap.end())
	{
		PrintDbg(lpszModuleName, nLine, TEXT("hwnd:0x%X:uMsg:0x%X:wParam:0x%X:lParam:0x%X"), hwnd, uMsg, wParam, lParam);
	}
	else
	{
		PrintDbg(lpszModuleName, nLine, TEXT("hwnd:0x%X:uMsg:%s:wParam:0x%X:lParam:0x%X"), hwnd, msgMap[uMsg].c_str(), wParam, lParam);
	}
}
