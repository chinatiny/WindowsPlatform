#ifndef PEOPERATION_H
#define PEOPERATION_H

#include <windows.h>

//判断是否是PE文件
BOOL IsPEFile(IN BYTE* pFileBuff);

//获取dos头
IMAGE_DOS_HEADER* GetDosHeader(IN BYTE* pFileBuff);

//获取NT头
IMAGE_NT_HEADERS* GetNtHeader(IN BYTE* pFileBuff);


//获取目录表
IMAGE_DATA_DIRECTORY* GetDirectory(IN BYTE* pFileBuff);

// 获取导出表
IMAGE_EXPORT_DIRECTORY* GetExportTable(IN BYTE* pFileBuff);

//获取导入表
IMAGE_IMPORT_DESCRIPTOR* GetImportTable(IN BYTE* pFileBuff);

//获取资源表
IMAGE_RESOURCE_DIRECTORY* GetResourceDirectory(IN BYTE* pFileBuff);

//获取重定位表
IMAGE_BASE_RELOCATION* GetBaseRelocation(IN BYTE* pFileBuff);

//获取延时导入表
IMAGE_DELAYLOAD_DESCRIPTOR* GetDelayLoadDescriptor(IN BYTE *pFileBuff);

//获取tls表
IMAGE_TLS_DIRECTORY* GetImageTls(IN BYTE *pFileBuff);

//修改基地址
BOOL ModifyImageBase(IN BYTE *pFileBuff, IN ULONG ulNewImageBase);

//增加一个分节
void AddSection(
	IN BYTE *pFileBuff,   //需要增加节的文件缓冲区
	IN int nOldFileBuffSize,  //需要增加节的文件缓冲区大小
	IN char* pszSectionName,  //需要增加的节名字(仅仅前7个字符有效)
	IN BYTE *pFillBuff, //在节中填充的数据
	IN int nFillSize,      //节中填充的大小
	IN DWORD dwCharacteristics, //新添加节的属性   IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE;
	OUT BYTE** newBuffOfAddress, //添加新节后缓冲区
	OUT int *pNewFileSize,   //新增加的节缓冲区大小
	OUT IMAGE_SECTION_HEADER *newSectionAddr //新增加的分节信息
	);

//设置分节属性
void SetSectionCharacteristics(IN BYTE *pFileBuff, IN DWORD dwCharacteristics);

//获取一个分节的属性
DWORD GetSectionCharacteristics(IN BYTE *pFileBuff, IN char* szSectionName);

//移动导入表到添加分节
void MoveImport(
	IN BYTE *pFileBuff, //需要移动的缓冲区 
	IN int nOldFileBuffSize, //需要移动节的文件缓冲区大小 
	IN char* pszSectionName, //需要增加的节名字(仅仅前7个字符有效) 
	IN DWORD dwCharacteristics, //新增分节的属性
	OUT BYTE** newBuffOfAddress, //添加新节后缓冲区 
	OUT int *pNewFileSize,//新增加的节缓冲区大小 
	OUT IMAGE_SECTION_HEADER *newSectionAddr //新增加的分节信息
	);

//添加导入表项和导入函数
void AddImportDll(
	IN BYTE *pFileBuff,
	IN char * szDllName, //添加的dll
	IN WORD wHint //导入序号
	);

//RVA 转换到Foa
BOOL RVA2Foa(IN BYTE* pFileBuff, IN ULONG ulRva, OUT ULONG *pulFileOffset);

//Foa 转换到 RVA
BOOL Foa2RVA(IN BYTE* pFileBuff, IN ULONG ulFileOffset, OUT ULONG *pulRva);


#endif