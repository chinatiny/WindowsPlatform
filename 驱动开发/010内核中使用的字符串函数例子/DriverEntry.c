#include <Ntifs.h>

/*
符号 	格式说明符 	类型
%c, %lc 	ANSI字符 	char
%C, %wc 	宽字符 	wchar_t
%d, %i 	十进制有符号整数 	int
%D 	十进制__int64 	__int64
%L 	十六进制的LARGE_INTEGER 	LARGE_INTEGER
%s, %ls 	NULL终止的ANSI字符串 	char*
%S, %ws 	NULL终止的宽字符串 	wchar_t*
%Z 	ANSI_STRING字符串 	ANSI_STRING
%wZ 	UNICODE_STRING字符串 	UNICODE_STRING
%u 	十进制的ULONG 	ULONG
%x 	小写字符十六进制的ULONG 	ULONG
%X 	大写字符十六进制的ULONG 	ULONG
%p 	指针Pointer 32/64位
*/
VOID DriverUnload(PDRIVER_OBJECT pDriver);

NTSTATUS DriverEntry(
	PDRIVER_OBJECT pDriver,
	PUNICODE_STRING pPath
	)
{
	pPath;
	pDriver->DriverUnload = DriverUnload;

	DbgBreakPoint();

	//初始化字符串,方式1
	UNICODE_STRING strTest = RTL_CONSTANT_STRING(L"Hello World");
	KdPrint(("RTL_CONSTANT_STRING:%wZ\n", strTest));

	//此处不能释放
	//RtlFreeUnicodeString(&strTest);

	//初始化字符串,方式2
	UNICODE_STRING strTest2;
	RtlInitUnicodeString(&strTest2, L"RtlInitUnicodeString");
	KdPrint(("RtlInitUnicodeString:%wZ\n", strTest2));


	//字符串拷贝
	UNICODE_STRING strTest3;
	WCHAR dst_buf[256];
	RtlInitEmptyUnicodeString(&strTest3, dst_buf, sizeof(dst_buf));
	RtlCopyUnicodeString(&strTest3, &strTest);
	KdPrint(("RtlCopyUnicodeString:%wZ\n", &strTest3));


	//粘接字符串
	RtlAppendUnicodeStringToString(&strTest3, &strTest);
	KdPrint(("RtlAppendUnicodeStringToString :%wZ\n", &strTest3));

	//字符串比较
	LONG lCompareResult = RtlCompareUnicodeString(&strTest2, &strTest3, TRUE);
	KdPrint(("RtlCompareUnicodeString :%d\n", lCompareResult));


	//字符串转换成数字
	UNICODE_STRING strToInteger = RTL_CONSTANT_STRING(L"9521123");
	ULONG uValue;
	RtlUnicodeStringToInteger(&strToInteger, 0, &uValue);
	KdPrint(("RtlUnicodeStringToInteger:%d\n", uValue));


	//数字转化成字符串
	UNICODE_STRING strInteger2;
	WCHAR buff[256] = { 0 };
	RtlInitEmptyUnicodeString(&strInteger2, buff, sizeof(buff));
	uValue = 12345678;
	RtlIntegerToUnicodeString(uValue, 10, &strInteger2);
	KdPrint(("RtlIntegerToUnicodeString:%wZ\n", &strInteger2));

	//转成ansi
	ANSI_STRING ansiString;
	RtlUnicodeStringToAnsiString(&ansiString, &strInteger2, TRUE);
	KdPrint(("RtlUnicodeStringToAnsiString:%Z\n", &ansiString));


	//释放字符串
	RtlFreeAnsiString(&ansiString);
	return STATUS_SUCCESS;
}

VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	pDriver;
}