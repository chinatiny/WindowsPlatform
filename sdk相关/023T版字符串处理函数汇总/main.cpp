#include <windows.h>
#include <tchar.h>

int main(int argc, char* argv[])
{
	//1. 求字符串长度
	TCHAR szTest[] = _T("你好中国");
	_tprintf(_T("len:%d\n"), _tcslen(szTest));

	//2. 字符拷贝
	TCHAR szSrc[MAX_PATH] = _T("Hello World");
	TCHAR szDst[MAX_PATH];
	_tcscpy_s(szDst, _countof(szDst), szSrc);
	_tprintf(_T("after tcscpy_s:%s\n"), szDst);

	//3. 字符串粘贴
	TCHAR szSrc2[MAX_PATH] = _T("Hello World");
	TCHAR szDst2[MAX_PATH] = _T("Hello Girl and Boys");
	_tcscat_s(szDst2, _countof(szDst2), szSrc2);
	_tprintf(_T("after _tcscat_s:%s\n"), szDst2);


	//4. 格式化缓冲区
	TCHAR sprintfBuff[MAX_PATH] = { 0 };
	_stprintf_s(sprintfBuff, _countof(sprintfBuff), _T("what can i do for you,  100 %s\n"),  _T("yuan"));
	_tprintf(_T("sprint out:%s\n"), sprintfBuff);

	//5. 字符串转化成数字
	TCHAR szHex[] = _T("0x16");
	//TCHAR* endPtr = &szHex[_tcslen(szHex)];
	int nValue1 = _tcstoul(szHex, NULL, 16);
	_tprintf(_T("value1 is:%d\n"), nValue1);

	//6.数字转字符串
	TCHAR szToaBuff[MAX_PATH];
	_itot_s(100, szToaBuff, _countof(szToaBuff), 16);
	_tprintf(_T("value2 is:%s\n"), szToaBuff);


	return 0;
}