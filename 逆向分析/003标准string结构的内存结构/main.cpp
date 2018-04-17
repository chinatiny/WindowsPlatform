#include <windows.h>
#include <string>

#define  String std::string

struct MyStdString 
{
#ifdef _DEBUG
	struct MyStdString** pSelf;
#endif
	union MyUnion
	{
		char* pString;
		char szString[16];
	}u;
	int nStringLen;
	int nBufSize;

	const char* GetBuff()
	{
		if (nStringLen >= sizeof(u.szString))
		{
			return u.pString;
		}
		else
		{
			return u.szString;
		}
	}
	void ModifyString(const char * szString)
	{
		if (nStringLen >= sizeof(u.szString))
		{
			strcpy_s(u.pString, nBufSize - 1, szString);
			nStringLen = strlen(szString);
		}
		else
		{
			strcpy_s(u.szString, sizeof(u.szString), szString);
			nStringLen = strlen(szString);
		}
	}

};


int main(int argc, char* argv[])
{

	std::string  s1 = "123";
	std::string  s2 = "123456";
	std::string  s3 = "123456789";
	std::string  s4 = "123456789123";
	std::string  s5 = "123456789123456";
	std::string  s6 = "123456789123456789";

	MyStdString* pString = (MyStdString*)&s6;
	pString->ModifyString("987654321");
	printf("s6=%s\n", s6.c_str());

	getchar();

	return 0;
}

