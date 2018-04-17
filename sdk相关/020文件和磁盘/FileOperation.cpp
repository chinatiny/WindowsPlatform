#include "FileOperation.h"
#include "Dbg.h"
#include <queue>
#pragma comment(lib,"shlwapi.lib")  
#include <shlwapi.h>  


//获取驱动器的简洁信息
void FileOperation::GetDriverInfo(std::vector<DriverData> OUT &vecDriver)
{
	TCHAR szAllDrive[MAX_FIND_PATH] = { 0 };
	GetLogicalDriveStrings(MAX_FIND_PATH, szAllDrive);
	for (int i = 0; szAllDrive[i] != _T('\0') && i < MAX_FIND_PATH; i++)
	{
		//获取盘符类型
		UINT uDriveType = GetDriveType(&szAllDrive[i]);
		if (!(DRIVE_NO_ROOT_DIR == uDriveType ||
			DRIVE_NO_ROOT_DIR == uDriveType ||
			DRIVE_REMOVABLE == uDriveType)
			)
		{
			DWORD dwSectorsPerCluster;      //每簇扇区数
			DWORD dwBytesPerSector;           //每个扇区多少字节
			DWORD dwNumberOfFreeClusters;  //多少空闲的簇
			DWORD dwTotalNumberOfClusters;  //一共有多少簇
			float fFreeMb;  //总共多少MB
			float fTotalMb; //总过多少MB
			//获取大小信息
			GetDiskFreeSpace(&szAllDrive[i], &dwSectorsPerCluster, &dwBytesPerSector, &dwNumberOfFreeClusters, &dwTotalNumberOfClusters);
			//
			fFreeMb = (float)dwSectorsPerCluster * dwBytesPerSector * dwNumberOfFreeClusters / 1024 / 1024;
			fTotalMb = (float)dwSectorsPerCluster * dwBytesPerSector * dwTotalNumberOfClusters / 1024 / 1024;
			//填充结构体信息
			DriverData driverData;
			_tcscpy_s(driverData.szDriverName, _countof(driverData.szDriverName), &szAllDrive[i]);
			driverData.uDriverType = uDriveType;
			driverData.dwSectorsPerCluster = dwSectorsPerCluster;
			driverData.dwBytesPerSector = dwBytesPerSector;
			driverData.dwNumberOfFreeClusters = dwNumberOfFreeClusters;
			driverData.dwTotalNumberOfClusters = dwTotalNumberOfClusters;
			driverData.fFreeMb = fFreeMb;
			driverData.fTotalMb = fTotalMb;

			PrintDbgInfo(_T("驱动器：%s, 总共：%lfMb, 剩余:%lfMb"), &szAllDrive[i], fTotalMb, fFreeMb);
			//存到向量中
			vecDriver.push_back(driverData);
		}
		i += _tcslen(&szAllDrive[i]);
	}
}

BOOL FileOperation::IsCdRoomHasMedia(IN LPCTSTR lpCdDriver)
{
	UINT uDriveType = GetDriveType(lpCdDriver);
	if (DRIVE_CDROM != uDriveType) return FALSE;
	if (21 == GetLastError()) return FALSE;
	return TRUE;
}

BOOL FileOperation::GetFileInfo(IN LPCTSTR lpPath, OUT BOOL &bIsDir, SYSTEMTIME &fileSt)
{
	TCHAR szFindPath[MAX_FIND_PATH] = { 0 };
	if (PathIsDirectory(lpPath))
	{
		_stprintf_s(szFindPath, MAX_FIND_PATH, TEXT("%s\\%s"), lpPath, _T("*"));
	}
	else
	{
		_tcscpy_s(szFindPath, _countof(szFindPath), lpPath);
	}
	//
	WIN32_FIND_DATA wfd;
	HANDLE hCheck = FindFirstFile(szFindPath, &wfd);
	if (INVALID_HANDLE_VALUE == hCheck) return FALSE;
	FindClose(hCheck);
	//获取系统时间
	FILETIME ft = {};
	FileTimeToLocalFileTime(&wfd.ftCreationTime, &ft);
	// 把文件时间转成系统时间
	FileTimeToSystemTime(&ft, &fileSt);
	//获取是否是目录
	bIsDir = wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? TRUE:FALSE;
	return TRUE;
}

BOOL FileOperation::GetFileSize(IN LPCTSTR lpPath, OUT DWORD &dwSizeLow, OUT DWORD &dwSizeHigh)
{
	//1.判断是否是一个普通文件
	OUT BOOL bIsDir;
	SYSTEMTIME fileSt;
	//如果不是文件
	if (!GetFileInfo(lpPath, bIsDir, fileSt)) return FALSE;
	//如果是目录
	if (bIsDir) return FALSE;
	
	//2.获取文件大小
	HANDLE hFile = ::CreateFile(
		lpPath,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	dwSizeHigh = 0;
	dwSizeLow = ::GetFileSize(hFile, &dwSizeHigh);
	return TRUE;
}

BOOL FileOperation::EnumFile(
	IN LPCTSTR lpRoot,
	OUT FileNode* &pRootNode,
	IN EnumFileCallBack fpCallBack,
	IN void* callBackParameter,
	IN int nMaxLevel,
	IN LPCTSTR lpExtenName)
{
	//1. 判断根节点是否存在
	BOOL bRootIsDir = FALSE;
	SYSTEMTIME rootSt = { 0 };
	if (!GetFileInfo(lpRoot, bRootIsDir, rootSt)) return FALSE;

	//2. 填充根节点
	pRootNode = new FileNode;
	pRootNode->bIsDir = bRootIsDir;
	_tcscpy_s(pRootNode->szFileName, _countof(pRootNode->szFileName), lpRoot);
	pRootNode->sysTime = rootSt;
	pRootNode->pParentFile = NULL;
	pRootNode->nLevel = 0;

	//3. 根据是否是否目录决定需要继续遍历
	if (!pRootNode->bIsDir) return TRUE;

	//4.开始遍历了
	int nLevelCount = 0;  //层次检查
	std::queue<FileNode*> enumQue;
	enumQue.push(pRootNode);
	while (!enumQue.empty())
	{
		//获取需要遍历的目录
		FileNode *pCurrentNode = enumQue.front();
		enumQue.pop();

		//构造遍历字符串
		TCHAR szFindPath[MAX_FIND_PATH] = { 0 };
		_stprintf_s(szFindPath, _countof(szFindPath), TEXT("%s\\%s"), pCurrentNode->szFileName, lpExtenName);

		//执行查找
		WIN32_FIND_DATA wfd = { 0 };
		HANDLE hFind = FindFirstFile(szFindPath, &wfd);
		if (INVALID_HANDLE_VALUE == hFind) continue;
		//
		do
		{
			//获取全路径名
			TCHAR szFullPath[MAX_FIND_PATH] = { 0 };
			_stprintf_s(szFullPath, _countof(szFullPath), TEXT("%s\\%s"), pCurrentNode->szFileName, wfd.cFileName);
			PrintDbgInfo(_T("当前文件:%s"), szFullPath);
			// 转换成本地时间
			FILETIME ft = {};
			FileTimeToLocalFileTime(&wfd.ftCreationTime, &ft);
			// 把文件时间转成系统时间
			SYSTEMTIME st = {};
			FileTimeToSystemTime(&ft, &st);

			// 如果是当前目录或上级目录  ：  . .. 就跳过
			if (!_tcscmp(wfd.cFileName, TEXT(".")) || !_tcscmp(wfd.cFileName, TEXT(".."))) continue;

			//填充节点
			FileNode *pNode = new FileNode;
			pNode->bIsDir =wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? TRUE : FALSE;
			_tcscpy_s(pNode->szFileName, _countof(pNode->szFileName), szFullPath);
			pNode->sysTime = st;
			pNode->pParentFile = pCurrentNode;
			pRootNode->nLevel;

			//放入到当前目录的子文件中
			pCurrentNode->vecSubFile.push_back(pNode);

			//执行回调函数
			if (NULL != fpCallBack)
			{
				//如果回调函数返回false继续，返回true结束遍历
				if (fpCallBack(callBackParameter, pNode)) break;
			}

			// 判断是否是目录
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				enumQue.push(pNode);
			}

		} while (FindNextFile(hFind, &wfd));
		FindClose(hFind);
		//
		++nLevelCount;
		//如果超过规定的层次，直接跳出，0代表直到遍历完毕
		if (nLevelCount >= nMaxLevel && nMaxLevel != 0) break;

	}
	return TRUE;
}

void FileOperation::DeleteFileNodeTree(OUT FileNode* &pRootNode)
{
	std::queue<FileNode*> enumQue;
	enumQue.push(pRootNode);

	//开始释放内存
	while (!enumQue.empty())
	{
		FileNode* pCurrentNode = enumQue.front();
		enumQue.pop();
		//
		for (size_t i = 0; i < pCurrentNode->vecSubFile.size(); i++)
		{
			FileNode* pSubNode = (pCurrentNode->vecSubFile)[i];
			enumQue.push(pSubNode);
		}
		delete pCurrentNode;
	}
	//
	pRootNode = NULL;

}

BOOL FileOperation::CopyFile(IN LPCTSTR lpExistingFileName, IN LPCTSTR lpNewFileName, IN BOOL bFailIfExists)
{
	return ::CopyFile(lpExistingFileName, lpNewFileName, bFailIfExists);
}

BOOL FileOperation::MoveFile(IN LPCTSTR lpExistingFileName, IN LPCTSTR lpNewFileName)
{
	return ::MoveFile(lpExistingFileName, lpNewFileName);
}

BOOL FileOperation::DeleteFile(IN LPCTSTR lpFileName)
{
	return ::DeleteFile(lpFileName);
}
