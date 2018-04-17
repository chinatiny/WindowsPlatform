/*
	全局构造和析构调用的地方的特征码:
	vs2015:
	全局构造函数:
	FF55F4EBCF8B4DFC33CD
	全局析构函数:
	FF55DC8B4DF48B118B028B0851



	*本程序取消了随机地址
	*本程序使用vs2013编译
*/

#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <vector>


class TestClass
{
public:
	TestClass()
	{
		printf("这个是全局对象的构造函数\n");
	}
	~TestClass()
	{
		printf("这个是全局对象的析构函数\n");
	}
};

TestClass testObj;



void MakeNext(BYTE matchCode[],  int nextArry[], int nMatchCodeLen)
{
	int i, k;
	nextArry[0] = 0;
	for (i = 1, k = 0; i < nMatchCodeLen; ++i)
	{
		while (k > 0 && ( matchCode[i] != matchCode[k] && matchCode[i] != '?'))
			k = nextArry[k - 1];
		if (matchCode[i] == matchCode[k] || matchCode[i] == '?' || matchCode[k] == '?')
		{
			k++;
		}
		nextArry[i] = k;
	}
}


std::vector<LPVOID> KMPSearch(BYTE Buff[], int nBuffLen, BYTE matchCode[], int matchCodeLen)
{
	std::vector<LPVOID> vecIndex;
	int *nextArry = new int[matchCodeLen];
	MakeNext(matchCode, nextArry, matchCodeLen);
	for (int i = 0,  j = 0; i < nBuffLen; ++i)
	{
		while (j > 0 && (matchCode[j] != Buff[i] && matchCode[j] != '?'))
			j = nextArry[j - 1];
		if (matchCode[j] == Buff[i] || matchCode[j] == '?')
		{
			j++;
		}
		if (j == matchCodeLen)
		{
			vecIndex.push_back((LPVOID)(i - matchCodeLen + 1 + (PBYTE)Buff));
			j = 0;
		}
	}
	delete[] nextArry;
	return vecIndex;
}


int main(int argc, char* argv[])
{
	//这个程序是静态编译的,动态编译的需要自己设置下
	HMODULE hMod = GetModuleHandle(NULL);
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hMod;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)hMod + pDosHeader->e_lfanew);

	//遍历全局对象的构造函数
	IMAGE_SECTION_HEADER* pSectionHeader = NULL;
	pSectionHeader = IMAGE_FIRST_SECTION(pNtHeader);
	DWORD dwRva = 0;
	DWORD dwSize = 0;
	for (ULONG i = 0; i < pNtHeader->FileHeader.NumberOfSections; ++i)
	{
		if (strcmp((const char*)pSectionHeader->Name, ".text") == 0)
		{
			dwRva = pSectionHeader->VirtualAddress;
			dwSize = pSectionHeader->SizeOfRawData;
			break;
		}
		pSectionHeader += 1;
	}

	BYTE byFindcode[] =
	{
		0x68, '?', '?', '?', '?',
		0x68, '?', '?', '?', '?',
		0xe8, '?', '?', '?', '?',
 		0x83, 0xc4, 0x08,
		0x83, 0x3d, '?', '?', '?', '?', 0x0,
 		0x74, '?', 0x68
	};


	std::vector<LPVOID> vecRet = KMPSearch((BYTE*)(DWORD)hMod + dwRva, dwSize, byFindcode, sizeof(byFindcode));

	for (size_t i = 0; i < vecRet.size(); i++)
	{
		printf("全局构造查找的位置:%08X\n", vecRet[i]);
	}

	BYTE opCode[10] = { 0 };
	memcpy(opCode, vecRet[0], sizeof(opCode));
	DWORD *pdwEnd = (DWORD*)(*(DWORD*)&opCode[1]);
	DWORD *pdwBegin = (DWORD*)(*(DWORD*)&opCode[6]);

	while (pdwBegin < pdwEnd)
	{
		DWORD dwAddr = *pdwBegin;
		if (dwAddr)
		{
			printf("全局对象构造函数地址:%08X\n", dwAddr);
		}
		pdwBegin++;
	}

	//遍历全局对象的析构函数
	BYTE byFindcode2[] =
	{
		0x8B, 0x0D, '?', '?', '?', '?',
 		0x51,
 		0xff, 0x15, '?', '?', '?', '?',
		0x89, 0x45, '?',
 		0x83, 0x7d, '?', 0x00,
 		0x0f
	};

	vecRet = KMPSearch((BYTE*)(DWORD)hMod + dwRva, dwSize, byFindcode2, sizeof(byFindcode2));

	for (size_t i = 0; i < vecRet.size(); i++)
	{
		printf("全局析构查找的位置:%08X\n", vecRet[i]);
	}

	BYTE opCode2[32] = { 0 };
	memcpy(opCode2, vecRet[0], 32);

	DWORD *pdwEnd2 = (DWORD*)(*(DWORD*)&opCode2[2]);
	DWORD *pdwBegin2 = (DWORD*)(*(DWORD*)&opCode2[28]);

	typedef PVOID(__stdcall *fpTypeRtlDecodePointer)(PVOID Pointer);
	HMODULE hNtdll = GetModuleHandle(_T("ntdll.dll"));
	fpTypeRtlDecodePointer  fpRtlDecodePointer = NULL;
	LPVOID lpResultAddr = NULL;
	fpRtlDecodePointer = (fpTypeRtlDecodePointer)GetProcAddress(hNtdll, "RtlDecodePointer");


	while (--pdwEnd2 >= pdwBegin2)
	{
		LPVOID lpAddr = fpRtlDecodePointer((PVOID)(*pdwEnd2));
		printf("全局的析构函数地址:%08X", lpAddr);
	}

	printf("obj addr:%08X\n", &testObj);
	system("pause");
	return 0;
}