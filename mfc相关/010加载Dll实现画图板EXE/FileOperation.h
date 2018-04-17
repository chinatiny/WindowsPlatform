#pragma once
#include <windows.h>
#include <tchar.h>
#include <vector>

//路径最长为
#define  MAX_FIND_PATH 1024

typedef struct
{
	UINT uDriverType;
	TCHAR szDriverName[MAX_FIND_PATH];
	DWORD dwSectorsPerCluster;      //每簇扇区数
	DWORD dwBytesPerSector;           //每个扇区多少字节
	DWORD dwNumberOfFreeClusters;  //多少空闲的簇
	DWORD dwTotalNumberOfClusters;  //一共有多少簇
	float fFreeMb;  //有多少空闲(MB)
	float fTotalMb; //总公共多少(MB)
}DriverData;

typedef struct __FileNode
{
	TCHAR szFileName[MAX_FIND_PATH];
	BOOL bIsDir;  //是否是目录
	SYSTEMTIME sysTime;  //文件创建时间
	__FileNode* pParentFile; //父节点是谁
	int nLevel; //当前遍历的层次
	std::vector<__FileNode*> vecSubFile; //如果当前文件为目录，目录的子文件
}FileNode;


//遍历文件的回调函数,如果bool函数返回false继续，返回true结束遍历
typedef BOOL (EnumFileCallBack)(void *Parameter, FileNode *pNode);

class FileOperation
{
public:
	//获取驱动器信息
	void GetDriverInfo(OUT std::vector<DriverData> &vecDriver);

	//判断光驱是否有光盘
	BOOL IsCdRoomHasMedia(IN LPCTSTR lpCdDriver);

	//获取文件信息,如果返回false代表不是一个文件
	BOOL GetFileInfo(IN LPCTSTR lpPath, OUT BOOL &bIsDir, SYSTEMTIME &fileSt);

	//获取文件大小,如果返回false代表不是一个文件或者是一个目录
	//dwSizeLow:  文件大小的低4个字节
	//dwSizeHigh： 文件大小的高四个字节
	BOOL GetFileSize(IN LPCTSTR lpPath, OUT DWORD &dwSizeLow, OUT DWORD &dwSizeHigh);

	//遍历文件的参数说明：
	//lpRoot:遍历的根节点
	//pRootNode:遍历后存储在树根
	//fpCallBack遍历文件的回调函数，如果bool函数返回false继续，返回true结束遍历，可以为空
	//callBackParameter回调函数的参数
	//nMaxLevel:最多多少层
	//lpExtenName：遍历文件匹配的扩展名
	BOOL EnumFile(
		IN LPCTSTR lpRoot,
		OUT FileNode* &pRootNode,
		IN EnumFileCallBack fpCallBack,
		IN void* callBackParameter,
		IN int nMaxLevel,
		IN LPCTSTR lpExtenName = _T("*"));

	//销毁因为遍历new出来的文件树
	void DeleteFileNodeTree(IN FileNode* &pRootNode);

	//文件拷贝
	BOOL CopyFile(
		IN  LPCTSTR lpExistingFileName,
		IN  LPCTSTR lpNewFileName,
		IN  BOOL bFailIfExists);

	//文件移动
	BOOL MoveFile(
		IN LPCTSTR lpExistingFileName,
		IN LPCTSTR lpNewFileName
		);

	//删除文件
	BOOL DeleteFile(IN LPCTSTR lpFileName);

};

