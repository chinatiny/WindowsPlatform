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
	_stprintf_s(pOutBuff, nMaxOutBuffSize - sizeof(TCHAR), _T("[mod:%s:line:%d]  ::  "), pFileName, nLine);
#else
	_tcscpy_s(fileNameBuff, lpszModuleName);
	TCHAR *pFileName = PathFindFileName(fileNameBuff);
	_stprintf_s(pOutBuff, nMaxOutBuffSize - sizeof(TCHAR), _T("[mod:%s:line:%d]  ::"), pFileName, nLine);
#endif

	//设置输入信息
	int nHeadLen = _tcslen(pOutBuff);
	_vstprintf_s(pOutBuff + nHeadLen, nMaxOutBuffSize - sizeof(TCHAR)-nHeadLen * sizeof(TCHAR), lptstrFormat, argList);

	//设置换行
	int nBodyLen = _tcslen(pOutBuff);
	_stprintf_s(pOutBuff + nBodyLen, nMaxOutBuffSize - sizeof(TCHAR)-nBodyLen * sizeof(TCHAR), _T("\r\n"));
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

	//输出信息
	OutputDebugString(outBuff);
}

#ifdef UNICODE
void InitWindowsMsgInfo(std::map<int, std::wstring> &msgMap, bool &bIsInit)
#else
void InitWindowsMsgInfo(std::map<int, std::wstring> &msgMap, bool &bIsInit)
#endif
{
	bIsInit = true;
	//填充消息结构
	msgMap.insert({ 0x00, _T("WM_NULL") });
	msgMap.insert({ 0x01, _T("WM_CREATE") });
	msgMap.insert({ 0x02, _T("WM_DESTROY") });
	msgMap.insert({ 0x03, _T("WM_MOVE") });
	msgMap.insert({ 0x05, _T("WM_SIZE") });
	msgMap.insert({ 0x06, _T("WM_ACTIVATE") });
	msgMap.insert({ 0x07, _T("WM_SETFOCUS") });
	msgMap.insert({ 0x08, _T("WM_KILLFOCUS") });
	msgMap.insert({ 0x0A, _T("WM_ENABLE") });
	msgMap.insert({ 0x0B, _T("WM_SETREDRAW") });
	msgMap.insert({ 0x0C, _T("WM_SETTEXT") });
	msgMap.insert({ 0x0D, _T("WM_GETTEXT") });
	msgMap.insert({ 0x0E, _T("WM_GETTEXTLENGTH") });
	msgMap.insert({ 0x0F, _T("WM_PAINT") });
	msgMap.insert({ 0x10, _T("WM_CLOSE") });
	msgMap.insert({ 0x11, _T("WM_QUERYENDSESSION") });
	msgMap.insert({ 0x12, _T("WM_QUIT") });
	msgMap.insert({ 0x13, _T("WM_QUERYOPEN") });
	msgMap.insert({ 0x14, _T("WM_ERASEBKGND") });
	msgMap.insert({ 0x15, _T("WM_SYSCOLORCHANGE") });
	msgMap.insert({ 0x16, _T("WM_ENDSESSION") });
	msgMap.insert({ 0x17, _T("WM_SYSTEMERROR") });
	msgMap.insert({ 0x18, _T("WM_SHOWWINDOW") });
	msgMap.insert({ 0x19, _T("WM_CTLCOLOR") });
	msgMap.insert({ 0x1A, _T("WM_WININICHANGE  OR WM_SETTINGCHANGE") });
	msgMap.insert({ 0x1B, _T("WM_DEVMODECHANGE") });
	msgMap.insert({ 0x1C, _T("WM_ACTIVATEAPP") });
	msgMap.insert({ 0x1D, _T("WM_FONTCHANGE") });
	msgMap.insert({ 0x1E, _T("WM_TIMECHANGE") });
	msgMap.insert({ 0x1F, _T("WM_CANCELMODE") });
	msgMap.insert({ 0x20, _T("WM_SETCURSOR") });
	msgMap.insert({ 0x21, _T("WM_MOUSEACTIVATE") });
	msgMap.insert({ 0x22, _T("WM_CHILDACTIVATE") });
	msgMap.insert({ 0x23, _T("WM_QUEUESYNC") });
	msgMap.insert({ 0x24, _T("WM_GETMINMAXINFO") });
	msgMap.insert({ 0x26, _T("WM_PAINTICON") });
	msgMap.insert({ 0x27, _T("WM_ICONERASEBKGND") });
	msgMap.insert({ 0x28, _T("WM_NEXTDLGCTL") });
	msgMap.insert({ 0x2A, _T("WM_SPOOLERSTATUS") });
	msgMap.insert({ 0x2B, _T("WM_DRAWITEM") });
	msgMap.insert({ 0x2C, _T("WM_MEASUREITEM") });
	msgMap.insert({ 0x2D, _T("WM_DELETEITEM") });
	msgMap.insert({ 0x2E, _T("WM_VKEYTOITEM") });
	msgMap.insert({ 0x2F, _T("WM_CHARTOITEM") });
	msgMap.insert({ 0x30, _T("WM_SETFONT") });
	msgMap.insert({ 0x31, _T("WM_GETFONT") });
	msgMap.insert({ 0x32, _T("WM_SETHOTKEY") });
	msgMap.insert({ 0x33, _T("WM_GETHOTKEY") });
	msgMap.insert({ 0x37, _T("WM_QUERYDRAGICON") });
	msgMap.insert({ 0x39, _T("WM_COMPAREITEM") });
	msgMap.insert({ 0x41, _T("WM_COMPACTING") });
	msgMap.insert({ 0x46, _T("WM_WINDOWPOSCHANGING") });
	msgMap.insert({ 0x47, _T("WM_WINDOWPOSCHANGED") });
	msgMap.insert({ 0x48, _T("WM_POWER") });
	msgMap.insert({ 0x4A, _T("WM_COPYDATA") });
	msgMap.insert({ 0x4B, _T("WM_CANCELJOURNAL") });
	msgMap.insert({ 0x4E, _T("WM_NOTIFY") });
	msgMap.insert({ 0x50, _T("WM_INPUTLANGCHANGEREQUEST") });
	msgMap.insert({ 0x51, _T("WM_INPUTLANGCHANGE") });
	msgMap.insert({ 0x52, _T("WM_TCARD") });
	msgMap.insert({ 0x53, _T("WM_HELP") });
	msgMap.insert({ 0x54, _T("WM_USERCHANGED") });
	msgMap.insert({ 0x55, _T("WM_NOTIFYFORMAT") });
	msgMap.insert({ 0x7B, _T("WM_CONTEXTMENU") });
	msgMap.insert({ 0x7C, _T("WM_STYLECHANGING") });
	msgMap.insert({ 0x7D, _T("WM_STYLECHANGED") });
	msgMap.insert({ 0x7E, _T("WM_DISPLAYCHANGE") });
	msgMap.insert({ 0x7F, _T("WM_GETICON") });
	msgMap.insert({ 0x80, _T("WM_SETICON") });
	msgMap.insert({ 0x81, _T("WM_NCCREATE") });
	msgMap.insert({ 0x82, _T("WM_NCDESTROY") });
	msgMap.insert({ 0x83, _T("WM_NCCALCSIZE") });
	msgMap.insert({ 0x84, _T("WM_NCHITTEST") });
	msgMap.insert({ 0x85, _T("WM_NCPAINT") });
	msgMap.insert({ 0x86, _T("WM_NCACTIVATE") });
	msgMap.insert({ 0x87, _T("WM_GETDLGCODE") });
	msgMap.insert({ 0xA0, _T("WM_NCMOUSEMOVE") });
	msgMap.insert({ 0xA1, _T("WM_NCLBUTTONDOWN") });
	msgMap.insert({ 0xA2, _T("WM_NCLBUTTONUP") });
	msgMap.insert({ 0xA3, _T("WM_NCLBUTTONDBLCLK") });
	msgMap.insert({ 0xA4, _T("WM_NCRBUTTONDOWN") });
	msgMap.insert({ 0xA5, _T("WM_NCRBUTTONUP") });
	msgMap.insert({ 0xA6, _T("WM_NCRBUTTONDBLCLK") });
	msgMap.insert({ 0xA7, _T("WM_NCMBUTTONDOWN") });
	msgMap.insert({ 0xA8, _T("WM_NCMBUTTONUP") });
	msgMap.insert({ 0xA9, _T("WM_NCMBUTTONDBLCLK") });
	msgMap.insert({ 0x100, _T("WM_KEYFIRST OR WM_KEYDOWN") });
	msgMap.insert({ 0x101, _T("WM_KEYUP") });
	msgMap.insert({ 0x102, _T("WM_CHAR") });
	msgMap.insert({ 0x103, _T("WM_DEADCHAR") });
	msgMap.insert({ 0x104, _T("WM_SYSKEYDOWN") });
	msgMap.insert({ 0x105, _T("WM_SYSKEYUP") });
	msgMap.insert({ 0x106, _T("WM_SYSCHAR") });
	msgMap.insert({ 0x107, _T("WM_SYSDEADCHAR") });
	msgMap.insert({ 0x108, _T("WM_KEYLAST") });
	msgMap.insert({ 0x10D, _T("WM_IME_STARTCOMPOSITION") });
	msgMap.insert({ 0x10E, _T("WM_IME_ENDCOMPOSITION") });
	msgMap.insert({ 0x10F, _T("WM_IME_COMPOSITION") });
	msgMap.insert({ 0x10F, _T("WM_IME_KEYLAST") });
	msgMap.insert({ 0x110, _T("WM_INITDIALOG") });
	msgMap.insert({ 0x111, _T("WM_COMMAND") });
	msgMap.insert({ 0x112, _T("WM_SYSCOMMAND") });
	msgMap.insert({ 0x113, _T("WM_TIMER") });
	msgMap.insert({ 0x114, _T("WM_HSCROLL") });
	msgMap.insert({ 0x115, _T("WM_VSCROLL") });
	msgMap.insert({ 0x116, _T("WM_INITMENU") });
	msgMap.insert({ 0x117, _T("WM_INITMENUPOPUP") });
	msgMap.insert({ 0x11F, _T("WM_MENUSELECT") });
	msgMap.insert({ 0x120, _T("WM_MENUCHAR") });
	msgMap.insert({ 0x121, _T("WM_ENTERIDLE") });
	msgMap.insert({ 0x132, _T("WM_CTLCOLORMSGBOX") });
	msgMap.insert({ 0x133, _T("WM_CTLCOLOREDIT") });
	msgMap.insert({ 0x134, _T("WM_CTLCOLORLISTBOX") });
	msgMap.insert({ 0x135, _T("WM_CTLCOLORBTN") });
	msgMap.insert({ 0x136, _T("WM_CTLCOLORDLG") });
	msgMap.insert({ 0x137, _T("WM_CTLCOLORSCROLLBAR") });
	msgMap.insert({ 0x138, _T("WM_CTLCOLORSTATIC") });
	msgMap.insert({ 0x200, _T("WM_MOUSEFIRST") });
	msgMap.insert({ 0x200, _T("WM_MOUSEMOVE") });
	msgMap.insert({ 0x201, _T("WM_LBUTTONDOWN") });
	msgMap.insert({ 0x202, _T("WM_LBUTTONUP") });
	msgMap.insert({ 0x203, _T("WM_LBUTTONDBLCLK") });
	msgMap.insert({ 0x204, _T("WM_RBUTTONDOWN") });
	msgMap.insert({ 0x205, _T("WM_RBUTTONUP") });
	msgMap.insert({ 0x206, _T("WM_RBUTTONDBLCLK") });
	msgMap.insert({ 0x207, _T("WM_MBUTTONDOWN") });
	msgMap.insert({ 0x208, _T("WM_MBUTTONUP") });
	msgMap.insert({ 0x209, _T("WM_MBUTTONDBLCLK") });
	msgMap.insert({ 0x20A, _T("WM_MOUSEWHEEL") });
	msgMap.insert({ 0x20E, _T("WM_MOUSEHWHEEL") });
	msgMap.insert({ 0x210, _T("WM_PARENTNOTIFY") });
	msgMap.insert({ 0x211, _T("WM_ENTERMENULOOP") });
	msgMap.insert({ 0x212, _T("WM_EXITMENULOOP") });
	msgMap.insert({ 0x213, _T("WM_NEXTMENU") });
	msgMap.insert({ 0x214, _T("WM_SIZING") });
	msgMap.insert({ 0x215, _T("WM_CAPTURECHANGED") });
	msgMap.insert({ 0x216, _T("WM_MOVING") });
	msgMap.insert({ 0x218, _T("WM_POWERBROADCAST") });
	msgMap.insert({ 0x219, _T("WM_DEVICECHANGE") });
	msgMap.insert({ 0x220, _T("WM_MDICREATE") });
	msgMap.insert({ 0x221, _T("WM_MDIDESTROY") });
	msgMap.insert({ 0x222, _T("WM_MDIACTIVATE") });
	msgMap.insert({ 0x223, _T("WM_MDIRESTORE") });
	msgMap.insert({ 0x224, _T("WM_MDINEXT") });
	msgMap.insert({ 0x225, _T("WM_MDIMAXIMIZE") });
	msgMap.insert({ 0x226, _T("WM_MDITILE") });
	msgMap.insert({ 0x227, _T("WM_MDICASCADE") });
	msgMap.insert({ 0x228, _T("WM_MDIICONARRANGE") });
	msgMap.insert({ 0x229, _T("WM_MDIGETACTIVE") });
	msgMap.insert({ 0x230, _T("WM_MDISETMENU") });
	msgMap.insert({ 0x231, _T("WM_ENTERSIZEMOVE") });
	msgMap.insert({ 0x232, _T("WM_EXITSIZEMOVE") });
	msgMap.insert({ 0x233, _T("WM_DROPFILES") });
	msgMap.insert({ 0x234, _T("WM_MDIREFRESHMENU") });
	msgMap.insert({ 0x281, _T("WM_IME_SETCONTEXT") });
	msgMap.insert({ 0x282, _T("WM_IME_NOTIFY") });
	msgMap.insert({ 0x283, _T("WM_IME_CONTROL") });
	msgMap.insert({ 0x284, _T("WM_IME_COMPOSITIONFULL") });
	msgMap.insert({ 0x285, _T("WM_IME_SELECT") });
	msgMap.insert({ 0x286, _T("WM_IME_CHAR") });
	msgMap.insert({ 0x290, _T("WM_IME_KEYDOWN") });
	msgMap.insert({ 0x291, _T("WM_IME_KEYUP") });
	msgMap.insert({ 0x2A1, _T("WM_MOUSEHOVER") });
	msgMap.insert({ 0x2A2, _T("WM_NCMOUSELEAVE") });
	msgMap.insert({ 0x2A3, _T("WM_MOUSELEAVE") });
	msgMap.insert({ 0x300, _T("WM_CUT") });
	msgMap.insert({ 0x301, _T("WM_COPY") });
	msgMap.insert({ 0x302, _T("WM_PASTE") });
	msgMap.insert({ 0x303, _T("WM_CLEAR") });
	msgMap.insert({ 0x304, _T("WM_UNDO") });
	msgMap.insert({ 0x305, _T("WM_RENDERFORMAT") });
	msgMap.insert({ 0x306, _T("WM_RENDERALLFORMATS") });
	msgMap.insert({ 0x307, _T("WM_DESTROYCLIPBOARD") });
	msgMap.insert({ 0x308, _T("WM_DRAWCLIPBOARD") });
	msgMap.insert({ 0x309, _T("WM_PAINTCLIPBOARD") });
	msgMap.insert({ 0x30A, _T("WM_VSCROLLCLIPBOARD") });
	msgMap.insert({ 0x30B, _T("WM_SIZECLIPBOARD") });
	msgMap.insert({ 0x30C, _T("WM_ASKCBFORMATNAME") });
	msgMap.insert({ 0x30D, _T("WM_CHANGECBCHAIN") });
	msgMap.insert({ 0x30E, _T("WM_HSCROLLCLIPBOARD") });
	msgMap.insert({ 0x30F, _T("WM_QUERYNEWPALETTE") });
	msgMap.insert({ 0x310, _T("WM_PALETTEISCHANGING") });
	msgMap.insert({ 0x311, _T("WM_PALETTECHANGED") });
	msgMap.insert({ 0x312, _T("WM_HOTKEY") });
	msgMap.insert({ 0x317, _T("WM_PRINT") });
	msgMap.insert({ 0x318, _T("WM_PRINTCLIENT") });
	msgMap.insert({ 0x358, _T("WM_HANDHELDFIRST") });
	msgMap.insert({ 0x35F, _T("WM_HANDHELDLAST") });
	msgMap.insert({ 0x380, _T("WM_PENWINFIRST") });
	msgMap.insert({ 0x38F, _T("WM_PENWINLAST") });
	msgMap.insert({ 0x390, _T("WM_COALESCE_FIRST") });
	msgMap.insert({ 0x39F, _T("WM_COALESCE_LAST") });
	msgMap.insert({ 0x3E0, _T("WM_DDE_FIRST") });
	msgMap.insert({ 0x3E0, _T("WM_DDE_INITIATE") });
	msgMap.insert({ 0x3E1, _T("WM_DDE_TERMINATE") });
	msgMap.insert({ 0x3E2, _T("WM_DDE_ADVISE") });
	msgMap.insert({ 0x3E3, _T("WM_DDE_UNADVISE") });
	msgMap.insert({ 0x3E4, _T("WM_DDE_ACK") });
	msgMap.insert({ 0x3E5, _T("WM_DDE_DATA") });
	msgMap.insert({ 0x3E6, _T("WM_DDE_REQUEST") });
	msgMap.insert({ 0x3E7, _T("WM_DDE_POKE") });
	msgMap.insert({ 0x3E8, _T("WM_DDE_EXECUTE") });
	msgMap.insert({ 0x3E8, _T("WM_DDE_LAST") });
	msgMap.insert({ 0x400, _T("WM_USER") });
	msgMap.insert({ 0x8000, _T("WM_APP") });
}
void PrintWindowsMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LPCSTR lpszModuleName, int nLine)
{
	static bool bIsInit = false;
#ifdef UNICODE
	static std::map<int, std::wstring> msgMap;
#else
	static std::map<int, std::wstring> msgMap;
#endif
	if (!bIsInit) InitWindowsMsgInfo(msgMap, bIsInit);

	if (msgMap.find(uMsg) == msgMap.end())
	{
		PrintDbg(lpszModuleName, nLine, _T("hwnd:0x%X:uMsg:0x%X:wParam:0x%X:lParam:0x%X"), hwnd, uMsg, wParam, lParam);
	}
	else
	{
		PrintDbg(lpszModuleName, nLine, _T("hwnd:0x%X:uMsg:%s:wParam:0x%X:lParam:0x%X"), hwnd, msgMap[uMsg].c_str(), wParam, lParam);
	}
}
