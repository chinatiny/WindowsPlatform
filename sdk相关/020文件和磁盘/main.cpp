#include "FileOperation.h"

int main(int argc, char* argv[])
{
	//1. 获取磁盘驱动器
	std::vector<DriverData> vecDriver;
	FileOperation fOperation;
	fOperation.GetDriverInfo(vecDriver);

	//2. 遍历文件
	FileNode *pRoot = NULL;
	fOperation.EnumFile(vecDriver[0].szDriverName, pRoot, NULL, NULL, 0);

	//3.释放遍历资源
	fOperation.DeleteFileNodeTree(pRoot);
}