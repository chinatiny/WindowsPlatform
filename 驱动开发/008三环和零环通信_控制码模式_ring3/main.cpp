#include <stdio.h>  
#include <windows.h>  
#include <winioctl.h>  

#define LINK_NAME L"\\\\.\\TestSys"  

#define  IN_BUFF_MAXLENGTH  0x10
#define  OUT_BUFFER_MAXLENGTH 0X10
#define OPER1 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800,METHOD_BUFFERED, FILE_ANY_ACCESS)  
#define OPER2 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801,METHOD_BUFFERED, FILE_ANY_ACCESS)   

HANDLE g_hDevice = NULL;

BOOL Open(TCHAR *pLinkName)
{
	TCHAR szBuff[10] = { 0 };
	BOOL retValue = FALSE;

	g_hDevice = CreateFile(
		pLinkName,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	printf("last error:%d\n", GetLastError());

	if (g_hDevice != INVALID_HANDLE_VALUE)
		retValue = TRUE;
	return retValue;
}

BOOL IoControl(DWORD dwIoCode, PVOID inBuff, DWORD inBuffLen, PVOID outBuff, DWORD outBuffLen)
{
	DWORD tmp;
	DeviceIoControl(
		g_hDevice,
		dwIoCode,
		inBuff,
		inBuffLen,
		outBuff,
		outBuffLen,
		&tmp,
		NULL);
	return TRUE;
}

int main(int argc, char* argv[])
{
	DWORD dwInBuffer = 0x7777777;
	TCHAR szOutBuffer[OUT_BUFFER_MAXLENGTH] = { 0 };

	Open(LINK_NAME);
	IoControl(OPER2, &dwInBuffer, IN_BUFF_MAXLENGTH, szOutBuffer, OUT_BUFFER_MAXLENGTH);
	printf("·µ»ØÊý¾Ý1:%s", szOutBuffer);
	CloseHandle(g_hDevice);
	return 0;
}

