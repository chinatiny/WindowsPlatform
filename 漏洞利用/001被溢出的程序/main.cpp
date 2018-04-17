#define  _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#define  PASSWORD "15PB"

int VerfiyPassword(char *pszPassword, int nSize)
{
	char szBuff[50];
	memcpy(szBuff, pszPassword, nSize);
	return strcmp(PASSWORD, szBuff);
}


int _tmain(int argc, _TCHAR* argv[])
{
	int nFlag = 0;
	char szPassword[0x1000];
	FILE *fp;
	LoadLibraryA("user32.dll");
	if (NULL == (fp = fopen("password.txt", "rb")))
	{
		MessageBoxA(NULL, "打开文件失败", "error", NULL);
		exit(0);
	}

	fread(szPassword, sizeof(szPassword), 1, fp);
	nFlag = VerfiyPassword(szPassword, sizeof(szPassword));

	if (nFlag) printf("密码错误");
	else printf("密码正确");

	fclose(fp);
	system("pause");
	return 0;
}

