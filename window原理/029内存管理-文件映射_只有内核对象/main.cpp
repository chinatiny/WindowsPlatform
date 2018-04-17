#include <windows.h>

int main()
{
	HANDLE hMapping;
	// 创建一个文件映射对象,可以不和任何文件进行关联
	// 在Global空间中创建对象的名称需要用到管理员权限
	hMapping = CreateFileMapping(INVALID_HANDLE_VALUE,/*文件句柄*/
		NULL,
		PAGE_READWRITE,
		0,
		4096,
		L"Global\\15pb_wenjianyingshe");

	// 将文件映射对象映射到进程的虚拟地址空间中
	LPBYTE pBuff =
		(LPBYTE)MapViewOfFile(hMapping,
		FILE_MAP_READ | FILE_MAP_WRITE,
		0, /*映射的偏移高32位*/
		0,/*映射的偏移低32位*/
		4096/*映射的字节数*/);

	UnmapViewOfFile(pBuff);
	CloseHandle(hMapping);

	return 0;
}