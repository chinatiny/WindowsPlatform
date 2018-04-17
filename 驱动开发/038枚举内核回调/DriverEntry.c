#pragma warning(disable:4101 4100 4055 4152 4047 4189 4022)

#include <ntddk.h>
#include <Ntstrsafe.h>


#ifdef _WIN64
#define PSP_MAX_CREATE_PROCESS_NOTIFY 64
#else
#define PSP_MAX_CREATE_PROCESS_NOTIFY 8
#endif
ULONG_PTR PspCreateProcessNotifyRoutineCount;
ULONG_PTR PspCreateProcessNotifyRoutine;

ULONG_PTR pPspSetCreateProcessNotifyRoutine;



DWORD                   g_OsVersion;                                            //系统版本  
//操作系统版本  
#define WINXP                   51  
#define WIN7                    61  
#define WIN8                    62  
#define WIN81                   63  
#define WIN10                   100  

//获取系统版本  
BOOLEAN GetOsVer(void);

//获取PspCreateProcessNotifyRoutineCount
ULONG_PTR GetPspCreateProcessNotifyRoutineCount(void);

//获取PspCreateProcessNotifyRoutine
ULONG_PTR GetPspCreateProcessNotifyRoutine(void);

//获取PspSetCreateProcessNotifyRoutine函数地址
ULONG_PTR GetPspSetCreateProcessNotifyRoutine(void);

//枚举移除CreateProcessNotify
NTSTATUS EnumRemoveCreateProcessNotify(void);

VOID CreateProcessNotify(HANDLE ParentId, HANDLE ProcessId, BOOLEAN Create);


VOID DriverUnload(IN PDRIVER_OBJECT DriverObject)
{
	PsSetCreateProcessNotifyRoutine((PCREATE_PROCESS_NOTIFY_ROUTINE)CreateProcessNotify, TRUE);
	return;
}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	DriverObject->DriverUnload = DriverUnload;
	PsSetCreateProcessNotifyRoutine((PCREATE_PROCESS_NOTIFY_ROUTINE)CreateProcessNotify, FALSE);
	KdPrint(("安装创建进程回调函数:%X\n", CreateProcessNotify));


	DbgBreakPoint();

	//枚举移除CreateProcessNotify
	EnumRemoveCreateProcessNotify();
	return STATUS_SUCCESS;
}

VOID CreateProcessNotify(HANDLE ParentId, HANDLE ProcessId, BOOLEAN Create)
{
	return;
}


//枚举移除CreateProcessNotify
NTSTATUS EnumRemoveCreateProcessNotify(void)
{
	ULONG i;
	PVOID MagicPtr, NotifyAddr;

	//获取系统版本
	if (GetOsVer() == FALSE)return STATUS_UNSUCCESSFUL;

	//获取PspSetCreateProcessNotifyRoutine函数地址
	if (pPspSetCreateProcessNotifyRoutine == NULL)
	{
		pPspSetCreateProcessNotifyRoutine = GetPspSetCreateProcessNotifyRoutine();
		if (pPspSetCreateProcessNotifyRoutine == NULL)return NULL;
	}

	//获取PspCreateProcessNotifyRoutineCount
	PspCreateProcessNotifyRoutineCount = GetPspCreateProcessNotifyRoutineCount();
	if (PspCreateProcessNotifyRoutineCount <= 0 || PspCreateProcessNotifyRoutineCount > PSP_MAX_CREATE_PROCESS_NOTIFY)return STATUS_UNSUCCESSFUL;


	//获取PspCreateProcessNotifyRoutine
	PspCreateProcessNotifyRoutine = GetPspCreateProcessNotifyRoutine();
	if (PspCreateProcessNotifyRoutine == NULL)return STATUS_UNSUCCESSFUL;


#ifdef _WIN64
	for (i = 0; i < PSP_MAX_CREATE_PROCESS_NOTIFY; i++)
		//for (i = 0; i < PspCreateProcessNotifyRoutineCount; i++)
	{
		MagicPtr = (PVOID)((PUCHAR)PspCreateProcessNotifyRoutine + i * sizeof(ULONG_PTR));
		if (MagicPtr == NULL)continue;

		NotifyAddr = *(PULONG_PTR)(MagicPtr);
		if (NotifyAddr == NULL)continue;

		if (MmIsAddressValid(NotifyAddr) && NotifyAddr != 0)
		{
			NotifyAddr = *(PULONG_PTR)(((ULONG_PTR)NotifyAddr & 0xfffffffffffffff0ui64) + sizeof(EX_RUNDOWN_REF));

			DbgPrint("LoadImageNotify at %llx", NotifyAddr);
			PsSetCreateProcessNotifyRoutine((PCREATE_PROCESS_NOTIFY_ROUTINE)NotifyAddr, TRUE);
		}
	}



#else

	for (i = 0; i < PSP_MAX_CREATE_PROCESS_NOTIFY; i++)
		//for (i = 0; i < PspCreateProcessNotifyRoutineCount; i++)
	{
		//PEX_CALLBACK_ROUTINE_BLOCK Point = (PEX_CALLBACK_ROUTINE_BLOCK)((Ref->Value >> 3) << 3);
		MagicPtr = (PVOID)((PUCHAR)PspCreateProcessNotifyRoutine + i * sizeof(ULONG_PTR));
		if (MagicPtr == NULL)continue;

		NotifyAddr = *(PULONG_PTR)(MagicPtr);
		if (NotifyAddr == NULL)continue;

		if (MmIsAddressValid(NotifyAddr) && NotifyAddr != 0)
		{
			//NotifyAddr = (ULONG)(Point->Function)
			NotifyAddr = *(PULONG_PTR)(((ULONG_PTR)NotifyAddr & 0xfffffff8) + sizeof(EX_RUNDOWN_REF));
			KdPrint(("枚举删除创建进程回调函数:%X\n", NotifyAddr));
			PsSetCreateProcessNotifyRoutine((PCREATE_PROCESS_NOTIFY_ROUTINE)NotifyAddr, TRUE);
		}
	}



#endif

	return STATUS_SUCCESS;
}

//获取PspSetCreateProcessNotifyRoutine函数地址
ULONG_PTR GetPspSetCreateProcessNotifyRoutine(void)
{
	ULONG_PTR i = 0;
	ULONG_PTR OffsetAddr = 0;
	LONG OffsetAddr64 = 0;
	UNICODE_STRING unstrFunc;
	ULONG_PTR pPsSetCreateProcessNotifyRoutine = NULL;
	RtlInitUnicodeString(&unstrFunc, L"PsSetCreateProcessNotifyRoutine");

	//获取函数地址
	pPsSetCreateProcessNotifyRoutine = (ULONG_PTR)MmGetSystemRoutineAddress(&unstrFunc);
	if (pPsSetCreateProcessNotifyRoutine == NULL)return 0;

#ifdef _WIN64
	switch (g_OsVersion)
	{
	case WIN7:
	case WIN81:
	case WIN10:
	{

				  for (i = pPsSetCreateProcessNotifyRoutine; i < pPsSetCreateProcessNotifyRoutine + 0xff; i++)
				  {
					  if (*(PUCHAR)i == 0x45 && *(PUCHAR)(i + 1) == 0x33 && *(PUCHAR)(i + 2) == 0xc0 && *(PUCHAR)(i + 3) == 0xe9)
					  {
						  RtlCopyMemory(&OffsetAddr64, (PUCHAR)(i + 4), sizeof(DWORD));
						  OffsetAddr = OffsetAddr64 + 8 + i;
						  pPspSetCreateProcessNotifyRoutine = OffsetAddr;
						  if (pPspSetCreateProcessNotifyRoutine == NULL)return NULL;
						  break;
					  }
				  }
	}
		break;
	case WIN8:
	{
				 //fffff802`d078b354 4533c0          xor     r8d, r8d
				 //fffff802`d078b357 eb07            jmp     nt!PspSetCreateProcessNotifyRoutine(fffff802`d078b360)
				 //fffff802`d078b359 90              nop
				 //fffff802`d078b35a 90              nop
				 //fffff802`d078b35b 90              nop

				 for (i = pPsSetCreateProcessNotifyRoutine; i < pPsSetCreateProcessNotifyRoutine + 0xff; i++)
				 {
					 if (*(PUCHAR)i == 0x45 && *(PUCHAR)(i + 1) == 0x33 && *(PUCHAR)(i + 2) == 0xc0 && *(PUCHAR)(i + 3) == 0xeb)
					 {
						 RtlCopyMemory(&OffsetAddr64, (PUCHAR)(i + 4), 1);
						 OffsetAddr = OffsetAddr64 + 5 + i;
						 pPspSetCreateProcessNotifyRoutine = OffsetAddr;
						 if (pPspSetCreateProcessNotifyRoutine == NULL)return NULL;
						 break;
					 }
				 }
	}
		break;
	default:
		break;
	}
#else
	//获取PspSetCreateProcessNotifyRoutine函数地址
	switch (g_OsVersion)
	{

	case WINXP:
		pPspSetCreateProcessNotifyRoutine = pPsSetCreateProcessNotifyRoutine;
		break;
	case WIN7:
	case WIN8:
	case WIN81:
	case WIN10:
	{
				  //83dd87eb e809000000      call    nt!PspSetCreateProcessNotifyRoutine(83dd87f9)
				  //83dd87f0 5d              pop     ebp
				  //83dd87f1 c20800          ret     8
				  for (i = pPsSetCreateProcessNotifyRoutine; i < pPsSetCreateProcessNotifyRoutine + 0xff; i++)
				  {
					  if (*(PUCHAR)i == 0xe8 && *(PUCHAR)(i + 5) == 0x5d && *(PUCHAR)(i + 6) == 0xc2 && *(PUCHAR)(i + 7) == 0x08 && *(PUCHAR)(i + 8) == 0x00)
					  {
						  RtlCopyMemory(&OffsetAddr, (PUCHAR)(i + 1), sizeof(ULONG_PTR));
						  pPspSetCreateProcessNotifyRoutine = i + OffsetAddr + 5;
						  if (pPspSetCreateProcessNotifyRoutine == NULL)return NULL;
						  break;
					  }
				  }
	}
		break;
	default:
		break;
	}
#endif

	return pPspSetCreateProcessNotifyRoutine;
}

//获取PspCreateProcessNotifyRoutineCount
ULONG_PTR GetPspCreateProcessNotifyRoutineCount(void)
{
	//定义变量
	ULONG_PTR i = 0;
	LONG OffsetAddr64 = 0;
	ULONG_PTR OffsetAddr = 0;
	ULONG_PTR RoutineCount = 0;
	PULONG_PTR pRoutineCountAdd = NULL;

#ifdef _WIN64
	switch (g_OsVersion)
	{
	case WIN7:
	case WIN8:
	case WIN81:
	case WIN10:
	{

				  //fffff800`040cb364 f0011d1996d6ff  lock add dword ptr [nt!PspCreateProcessNotifyRoutineCount (fffff800`03e34984)],ebx
				  for (i = pPspSetCreateProcessNotifyRoutine; i < pPspSetCreateProcessNotifyRoutine + 0x256; i++)
				  {
					  if (*(PUCHAR)i == 0xf0 && *(PUCHAR)(i + 1) == 0x01)
					  {
						  RtlCopyMemory(&OffsetAddr64, (PUCHAR)(i + 3), sizeof(DWORD));
						  OffsetAddr = OffsetAddr64 + 7 + i;
						  break;
					  }
				  }


	}
		break;
	default:
		break;
	}

#else
	switch (g_OsVersion)
	{
	case WINXP:
	{
				  //805c6e59 b8ffffffff      mov     eax, 0FFFFFFFFh
				  //805c6e5e b900b35580      mov     ecx, offset nt!PspCreateProcessNotifyRoutineCount(8055b300)
				  for (i = pPspSetCreateProcessNotifyRoutine; i < pPspSetCreateProcessNotifyRoutine + 0xff; i++)
				  {
					  if (*(PUCHAR)i == 0xb8 && *(PUCHAR)(i + 1) == 0xff && *(PUCHAR)(i + 2) == 0xff && *(PUCHAR)(i + 3) == 0xff && *(PUCHAR)(i + 4) == 0xff && *(PUCHAR)(i + 5) == 0xb9)
					  {
						  RtlCopyMemory(&OffsetAddr, (PUCHAR)(i + 6), sizeof(ULONG_PTR));
						  break;
					  }
				  }
	}
		break;
	case WIN7:
	{
				 //83dd88a6 83c9ff or ecx, 0FFFFFFFFh
				 //83dd88a9 807d1000        cmp     byte ptr[ebp + 10h], 0
				 //83dd88ad b8e46dd883      mov     eax, offset nt!PspCreateProcessNotifyRoutineCount(83d86de4)
				 for (i = pPspSetCreateProcessNotifyRoutine; i < pPspSetCreateProcessNotifyRoutine + 0xff; i++)
				 {
					 if (*(PUCHAR)i == 0x83 && *(PUCHAR)(i + 1) == 0xc9 && *(PUCHAR)(i + 2) == 0xff && *(PUCHAR)(i + 7) == 0xb8)
					 {
						 RtlCopyMemory(&OffsetAddr, (PUCHAR)(i + 8), sizeof(ULONG_PTR));
						 break;
					 }
				 }

	}
		break;
	case WIN8:
	case WIN81:
	{
				  //8173a869 b81cc75f81      mov     eax, offset nt!PspCreateProcessNotifyRoutineCount(815fc71c)
				  //8173a86e f00fc118        lock xadd dword ptr[eax], ebx
				  for (i = pPspSetCreateProcessNotifyRoutine; i < pPspSetCreateProcessNotifyRoutine + 0xff; i++)
				  {
					  if (*(PUCHAR)i == 0xb8 && *(PUCHAR)(i + 5) == 0xf0 && *(PUCHAR)(i + 6) == 0x0f && *(PUCHAR)(i + 7) == 0xc1)
					  {
						  RtlCopyMemory(&OffsetAddr, (PUCHAR)(i + 1), sizeof(ULONG_PTR));
						  break;
					  }
				  }
	}
		break;
	case WIN10:
	{
				  //819c3a62 f0ff05d82dba81  lock inc dword ptr[nt!PspCreateProcessNotifyRoutineCount(81ba2dd8)]
				  for (i = pPspSetCreateProcessNotifyRoutine; i < pPspSetCreateProcessNotifyRoutine + 0xff; i++)
				  {
					  if (*(PUCHAR)i == 0xf0 && *(PUCHAR)(i + 1) == 0xff && *(PUCHAR)(i + 2) == 0x05)
					  {
						  RtlCopyMemory(&OffsetAddr, (PUCHAR)(i + 3), sizeof(ULONG_PTR));
						  break;
					  }
				  }

	}
		break;
	default:
		return 0;
	}
#endif

	if (OffsetAddr && MmIsAddressValid(OffsetAddr))
	{
		RoutineCount = *(PULONG)(OffsetAddr);
		//RoutineCount = *(PULONG_PTR)(OffsetAddr);
	}
	return RoutineCount;
}

//获取PspCreateProcessNotifyRoutine
ULONG_PTR GetPspCreateProcessNotifyRoutine(void)
{
	//定义变量
	ULONG_PTR i = 0;
	LONG OffsetAddr64 = 0;
	ULONG_PTR OffsetAddr = 0;
	ULONG_PTR NotifyRoutine = 0;

#ifdef _WIN64

	switch (g_OsVersion)
	{
	case WIN7:
	case WIN8:
	case WIN81:
	case WIN10:
	{
				  ////fffff800`040cb1f6 4c8d358395d6ff  lea     r14, [nt!PspCreateProcessNotifyRoutine(fffff800`03e34780)]
				  for (i = pPspSetCreateProcessNotifyRoutine; i < pPspSetCreateProcessNotifyRoutine + 0x256; i++)
				  {
					  if (*(PUCHAR)i == 0x4c && *(PUCHAR)(i + 1) == 0x8d)
					  {
						  RtlCopyMemory(&OffsetAddr64, (PUCHAR)(i + 3), sizeof(DWORD));
						  OffsetAddr = OffsetAddr64 + 7 + i;
						  break;
					  }
				  }

	}
		break;
	default:
		break;
	}


#else

	switch (g_OsVersion)
	{
	case WINXP:
	{
				  //805c6e19 bfe0b25580      mov     edi, offset nt!PspCreateProcessNotifyRoutine(8055b2e0)
				  for (i = pPspSetCreateProcessNotifyRoutine; i < pPspSetCreateProcessNotifyRoutine + 0xff; i++)
				  {
					  if (*(PUCHAR)i == 0xbf)
					  {
						  RtlCopyMemory(&OffsetAddr, (PUCHAR)(i + 1), sizeof(ULONG_PTR));
						  break;
					  }
				  }

	}
		break;
	case WIN7:
	{
				 //83dd8819 33db            xor     ebx, ebx
				 //83dd881b c7450ce06cd883  mov     dword ptr[ebp + 0Ch], offset nt!PspCreateProcessNotifyRoutine(83d86ce0)
				 for (i = pPspSetCreateProcessNotifyRoutine; i < pPspSetCreateProcessNotifyRoutine + 0xff; i++)
				 {
					 if (*(PUCHAR)i == 0x33 && *(PUCHAR)(i + 1) == 0xdb && *(PUCHAR)(i + 2) == 0xc7 && *(PUCHAR)(i + 3) == 0x45)
					 {
						 RtlCopyMemory(&OffsetAddr, (PUCHAR)(i + 5), sizeof(ULONG_PTR));
						 break;
					 }
				 }
	}
		break;
	case WIN8:
	{

				 //8173a82e b840c75f81      mov     eax, offset nt!PspCreateProcessNotifyRoutine(815fc740)
				 //8173a833 33f6            xor     esi, esi

				 for (i = pPspSetCreateProcessNotifyRoutine; i < pPspSetCreateProcessNotifyRoutine + 0xff; i++)
				 {
					 if (*(PUCHAR)i == 0xb8 && *(PUCHAR)(i + 5) == 0x33 && *(PUCHAR)(i + 6) == 0xf6)
					 {
						 RtlCopyMemory(&OffsetAddr, (PUCHAR)(i + 1), sizeof(ULONG_PTR));
						 break;
					 }
				 }

	}
		break;
	case WIN81:
	{

				  //81765890 b808d36081      mov     eax, offset nt!PspCreateProcessNotifyRoutine(8160d308)
				  //81765895 8bdf            mov     ebx, edi
				  for (i = pPspSetCreateProcessNotifyRoutine; i < pPspSetCreateProcessNotifyRoutine + 0xff; i++)
				  {
					  if (*(PUCHAR)i == 0xb8 && *(PUCHAR)(i + 5) == 0x8b && *(PUCHAR)(i + 6) == 0xdf)
					  {
						  RtlCopyMemory(&OffsetAddr, (PUCHAR)(i + 1), sizeof(ULONG_PTR));
						  break;
					  }
				  }

	}
		break;
	case WIN10:
	{
				  //819c3a31 bb304c8681      mov     ebx, offset nt!PspCreateProcessNotifyRoutine(81864c30)
				  //819c3a36 8bf7            mov     esi, edi
				  for (i = pPspSetCreateProcessNotifyRoutine; i < pPspSetCreateProcessNotifyRoutine + 0xff; i++)
				  {
					  if (*(PUCHAR)i == 0xbb && *(PUCHAR)(i + 5) == 0x8b && *(PUCHAR)(i + 6) == 0xf7)
					  {
						  RtlCopyMemory(&OffsetAddr, (PUCHAR)(i + 1), sizeof(ULONG_PTR));
						  break;
					  }
				  }

	}
		break;
	default:
		break;
	}
#endif

	if (OffsetAddr && MmIsAddressValid(OffsetAddr))
	{
		NotifyRoutine = OffsetAddr;
	}
	return NotifyRoutine;
}


//获取系统版本  
BOOLEAN GetOsVer(void)
{

	ULONG    dwMajorVersion = 0;
	ULONG    dwMinorVersion = 0;
	PsGetVersion(&dwMajorVersion, &dwMinorVersion, NULL, NULL);
	if (dwMajorVersion == 5 && dwMinorVersion == 1)
		g_OsVersion = WINXP;
	else if (dwMajorVersion == 6 && dwMinorVersion == 1)
		g_OsVersion = WIN7;
	else if (dwMajorVersion == 6 && dwMinorVersion == 2)
		g_OsVersion = WIN8;
	else if (dwMajorVersion == 6 && dwMinorVersion == 3)
		g_OsVersion = WIN81;
	else if (dwMajorVersion == 10 && dwMinorVersion == 0)
		g_OsVersion = WIN10;
	else
	{
		g_OsVersion = 0;
		KdPrint(("未知版本"));
		return FALSE;
	}

	return TRUE;
}
