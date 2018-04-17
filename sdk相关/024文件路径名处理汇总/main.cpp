#include <windows.h>
#include <tchar.h>

#include <shlwapi.h>  
#pragma comment(lib,"shlwapi.lib")  


int main(int argc, char* argv[])
{
	//1.去除路径最后的反斜杠“\”
	TCHAR szFullPath[] = _T("C:\\Windows\\System32\\oobe\\zh-CN\\audit.exe.mui\\");
	PathRemoveBackslash(szFullPath);
	_tprintf(_T("PathRemoveBackslash %s\n"), szFullPath);

	//2.在路径最后加上反斜杠“\”
	PathAddBackslash(szFullPath);
	_tprintf(_T("PathAddBackslash %s\n"), szFullPath);
	PathRemoveBackslash(szFullPath); 

	//3. 删除文件的扩展名
	PathRemoveExtension(szFullPath);
	_tprintf(_T("PathRemoveExtension %s\n"), szFullPath);

	//4. 在文件路径后面加上扩展名
	PathAddExtension(szFullPath, _T("exe"));
	_tprintf(_T("PathAddExtension %s\n"), szFullPath);

	//5.更改文件路径扩展名
	PathRenameExtension(szFullPath, _T(".mui"));
	_tprintf(_T("PathRenameExtension %s\n"), szFullPath);

	//6. 去除文件名，得到目录
	PathRemoveFileSpec(szFullPath);
	_tprintf(_T("PathRemoveFileSpec %s\n"), szFullPath);

	//7. 去除目录得到文件名
	TCHAR szFullPath2[] = _T("C:\\Windows\\System32\\oobe\\zh-CN\\audit.exe.mui");
	PathStripPath(szFullPath2);
	_tprintf(_T("PathStripPath %s\n"), szFullPath2);

	//8.去掉路径的文件部分，得到根目录
	TCHAR szFullPath3[] = _T("C:\\Windows\\System32\\oobe\\zh-CN\\audit.exe.mui");
	PathStripToRoot(szFullPath3);
	_tprintf(_T("PathStripToRoot %s\n"), szFullPath3);

	//9. 获取文件的扩展名
	TCHAR szFullPath4[] = _T("C:\\Windows\\System32\\oobe\\zh-CN\\audit.exe.mui");
	TCHAR* szExtension =PathFindExtension(szFullPath4);
	_tprintf(_T("PathFindExtension %s\n"), szExtension);

	return 0;
}