#include <windows.h>
#include <winternl.h>
#include <stdio.h>
#pragma comment(lib,"ntdll.lib")

// 对象类型信息结构体
typedef struct _OBJECT_TYPE_INFORMATION {
	UNICODE_STRING TypeName; // 类型名
	ULONG TotalNumberOfHanders;
	ULONG TotalNumberOfObjects;
}OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

// 对象类型信息数组
typedef struct _OBJECT_ALL_INFORMATION {
	ULONG NumberOfObjectsTypes; // 数组元素个数
	OBJECT_TYPE_INFORMATION ObjectTypeInfo[1];
}OBJECT_ALL_INFORMATION, *POBJECT_ALL_INFORMATION;


bool NQO__NtQueryObject()
{
	// 1. 获取欲查询信息大小
	ULONG uSize = 0;
	NtQueryObject(NULL,
		(OBJECT_INFORMATION_CLASS)0x03,
		&uSize,
		sizeof(uSize),
		&uSize);

	// 2. 获取对象信息
	POBJECT_ALL_INFORMATION pObjectAllInfo =
		(POBJECT_ALL_INFORMATION)new BYTE[uSize];

	NtQueryObject(NULL,
		(OBJECT_INFORMATION_CLASS)0x03,
		pObjectAllInfo,
		uSize,
		&uSize);


	// 3. 循环遍历并处理对象信息
	POBJECT_TYPE_INFORMATION pObjType = pObjectAllInfo->ObjectTypeInfo;

	for (ULONG i = 0; i < pObjectAllInfo->NumberOfObjectsTypes; i++)
	{
		wprintf(L"%s\n", pObjType->TypeName.Buffer);

		// 3.1 查看此对象的类型是否为DebugObject，还需要判断对象的数量，大于0则说明有调试对象
		if (wcscmp(L"DebugObject", pObjType->TypeName.Buffer) == 0)
		{
			return pObjType->TotalNumberOfObjects > 0;
		}



		// 3.2 获取对象名占用空间的大小（考虑到了结构体对齐问题）
		ULONG uNameLength = pObjType->TypeName.Length;
		ULONG uDataLength = uNameLength - uNameLength % sizeof(ULONG)+sizeof(ULONG);


		// 3.3 指向下一个对象信息
		pObjType = (POBJECT_TYPE_INFORMATION)pObjType->TypeName.Buffer;
		pObjType = (POBJECT_TYPE_INFORMATION)((PBYTE)pObjType + uDataLength);

	}


	delete[] pObjectAllInfo;
	return false;
}

//
int main(int argc, char* argv[])
{
	bool bDebug = NQO__NtQueryObject();
	if (bDebug)
	{
		MessageBoxA(NULL, "正在被调试", "警告", 0);
	}
	else
	{
		MessageBoxA(NULL, "现在很安全", "恭喜", 0);
	}

	return 0;
}

