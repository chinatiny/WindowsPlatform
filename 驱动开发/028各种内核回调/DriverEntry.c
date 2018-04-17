#pragma warning(disable:4101 4100 4055 4152 4214 4127 4057)
#include <ntddk.h>

#define MAX_PATH 256
void  DriverUnload(__in struct _DRIVER_OBJECT  *DriverObject);

VOID LoadImageNotifyRoutine(IN PUNICODE_STRING  FullImageName,
	IN HANDLE  ProcessId, // where image is mapped
	IN PIMAGE_INFO  ImageInfo
	);
NTSTATUS PsLookupProcessByProcessId(
	_In_  HANDLE    ProcessId,
	_Out_ PEPROCESS *Process
	);

NTSTATUS PsReferenceProcessFilePointer(
	IN PEPROCESS pProcess,
	OUT PFILE_OBJECT* ppFileObject);
VOID GetProcessPath(ULONG eprocess, PUNICODE_STRING pFilePath);

VOID ProcessNotifyRoutine(
	IN HANDLE        ParentId,
	IN HANDLE        ProcessId,
	IN BOOLEAN        Create
	);

VOID CreateThreadNotify(
	IN HANDLE  ProcessId,
	IN HANDLE  ThreadId,
	IN BOOLEAN  Create
	);


BOOLEAN IsAbsolute(PREG_CREATE_KEY_INFORMATION pCreateInfo);

NTSTATUS  RegistryCallback(__in PVOID  CallbackContext,
	__in_opt PVOID  Argument1,
	__in_opt PVOID  Argument2);

NTSTATUS _stdcall ObQueryNameString(__in PVOID Object,
	__out_bcount_opt(Length) POBJECT_NAME_INFORMATION ObjectNameInfo,
	__in ULONG Length,
	__out PULONG ReturnLength);

UCHAR * _stdcall PsGetProcessImageFileName(__in PEPROCESS Process);

LARGE_INTEGER	g_CmCallbackCookies = { 0 };


NTSTATUS g_statusProcessNotify = STATUS_UNSUCCESSFUL;
NTSTATUS g_statusLoadImageNotify = STATUS_UNSUCCESSFUL;
NTSTATUS g_statusRegNotify = STATUS_UNSUCCESSFUL;
NTSTATUS g_statusCreateThread = STATUS_UNSUCCESSFUL;

NTSTATUS DriverEntry(__in struct _DRIVER_OBJECT* DriverObject, __in PUNICODE_STRING RegistryPath)
{
	DriverObject->DriverUnload = DriverUnload;
	g_statusLoadImageNotify = PsSetLoadImageNotifyRoutine(LoadImageNotifyRoutine);
	g_statusProcessNotify = PsSetCreateProcessNotifyRoutine(ProcessNotifyRoutine, FALSE);
	g_statusRegNotify = CmRegisterCallback(RegistryCallback, NULL, &g_CmCallbackCookies);
	g_statusCreateThread = PsSetCreateThreadNotifyRoutine(CreateThreadNotify);
	return STATUS_SUCCESS;
}


void  DriverUnload(__in struct _DRIVER_OBJECT  *DriverObject)
{
	if (STATUS_SUCCESS == g_statusProcessNotify) 	PsSetCreateProcessNotifyRoutine(ProcessNotifyRoutine, TRUE);
	if (STATUS_SUCCESS == g_statusLoadImageNotify) PsRemoveLoadImageNotifyRoutine(LoadImageNotifyRoutine);
	if (STATUS_SUCCESS == g_statusRegNotify)  	CmUnRegisterCallback(g_CmCallbackCookies);
	if (STATUS_SUCCESS == g_statusCreateThread) PsRemoveCreateThreadNotifyRoutine(CreateThreadNotify);
}


VOID LoadImageNotifyRoutine(IN PUNICODE_STRING  FullImageName,
	IN HANDLE  ProcessId, // where image is mapped
	IN PIMAGE_INFO  ImageInfo
	)
{
	PIMAGE_INFO_EX pInfo = NULL;
	if (!FullImageName || !ImageInfo)
	{
		return;
	}
	if (ImageInfo->ExtendedInfoPresent)
	{
		pInfo = CONTAINING_RECORD(ImageInfo, IMAGE_INFO_EX, ImageInfo);
		DbgPrint("ModLoad Name = %wZ,ProcessID = 0x%x,FileObj = 0x%x,ImageBase = 0x%x,Size = 0x%x\n",
			FullImageName, ProcessId, pInfo->FileObject, pInfo->ImageInfo.ImageBase, pInfo->ImageInfo.ImageSize);
	}
	return;
}

VOID GetProcessPath(ULONG eprocess, PUNICODE_STRING pFilePath)
{
	PFILE_OBJECT        FilePointer = NULL;
	UNICODE_STRING        name;  //盘符
	NTSTATUS                status = STATUS_SUCCESS;
	status = PsReferenceProcessFilePointer((PEPROCESS)eprocess, &FilePointer);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("break out");
	}

	ObReferenceObjectByPointer(
		(PVOID)FilePointer,
		0,
		NULL,
		KernelMode);
	RtlVolumeDeviceToDosName((FilePointer)->DeviceObject, &name); //获取盘符名
	RtlCopyUnicodeString(pFilePath, &name); //盘符连接
	RtlAppendUnicodeStringToString(pFilePath, &(FilePointer)->FileName); //路径连接
	ObDereferenceObject(FilePointer);                //关闭对象引用
}

VOID ProcessNotifyRoutine(
	IN HANDLE        ParentId,
	IN HANDLE        ProcessId,
	IN BOOLEAN        Create
	)
{
	NTSTATUS                status = STATUS_SUCCESS;
	PEPROCESS                pEprocess = NULL;
	UNICODE_STRING         uniPath = { 0 };

	UNREFERENCED_PARAMETER(ParentId);

	uniPath.Length = 0;
	uniPath.MaximumLength = MAX_PATH * 2;
	uniPath.Buffer = (PWSTR)ExAllocatePool(NonPagedPool, uniPath.MaximumLength);

	// 创建进程
	if (Create)
	{
		DbgPrint("*******----有----新----进----程----创----建----*******\r\n");
		// 父进程
		// DbgPrint("父进程信息\r\n");
		// DbgPrint("        PID:  %d\r\n", ParentId);
		// status = PsLookupProcessByProcessId(ParentId, &pEprocess);
		// if (NT_SUCCESS(status))
		// {
		// GetProcessPath(pEprocess, &uniPath);
		// DbgPrint("        路径: %wZ\r\n", &uniPath);
		// }
		// 进程
		DbgPrint("创建进程信息\r\n");
		DbgPrint("        PID:  %d\r\n", ProcessId);
		status = PsLookupProcessByProcessId(ProcessId, &pEprocess);
		if (NT_SUCCESS(status))
		{
			GetProcessPath((ULONG)pEprocess, &uniPath);
			DbgPrint("        路径: %wZ\r\n", &uniPath);
		}
	}
	// 结束进程
	else
	{
		DbgPrint("*******----有----旧----进----程----退----出----*******\r\n");
		// 父进程
		//DbgPrint("父进程信息\r\n");
		//DbgPrint("        PID:  %d\r\n", ParentId);
		//status = PsLookupProcessByProcessId(ParentId, &pEprocess);
		//if (NT_SUCCESS(status))
		//{
		//        GetProcessPath(pEprocess, &uniPath);
		//        DbgPrint("        路径: %wZ\r\n", &uniPath);
		//}
		// 进程
		DbgPrint("退出进程信息\r\n");
		DbgPrint("        PID:  %d\r\n", ProcessId);
		status = PsLookupProcessByProcessId(ProcessId, &pEprocess);
		if (NT_SUCCESS(status))
		{
			GetProcessPath((ULONG)pEprocess, &uniPath);
			DbgPrint("        路径: %wZ\r\n", &uniPath);
		}
	}
	ExFreePool(uniPath.Buffer);
}


NTSTATUS  RegistryCallback(
	__in PVOID  CallbackContext,
	__in_opt PVOID  Argument1,
	__in_opt PVOID  Argument2)
{
	switch ((REG_NOTIFY_CLASS)Argument1)
	{
	case RegNtPreCreateKey:
	{
							  CHAR *pProcessName = (CHAR*)PsGetProcessImageFileName(PsGetCurrentProcess());
							  PREG_PRE_CREATE_KEY_INFORMATION pCreateInfo = (PREG_PRE_CREATE_KEY_INFORMATION)Argument2;
							  DbgPrint("RegFilter ProcessName = %s,CreateKey:%wZ\n", pProcessName, pCreateInfo->CompleteName);
							  break;
	}
	case RegNtPreCreateKeyEx:
	{
								CHAR *pProcessName = PsGetProcessImageFileName(PsGetCurrentProcess());
								PREG_CREATE_KEY_INFORMATION pCreateInfo = (PREG_CREATE_KEY_INFORMATION)Argument2;
								/*判断是否绝对路径*/
								if (IsAbsolute(pCreateInfo))
								{
									/*绝对路径*/
									DbgPrint("RegFilter ProcessName = %s,CreateKeyEx:%wZ\n", pProcessName, pCreateInfo->CompleteName);
								}
								else
								{
									CHAR strrRootPath[MAX_PATH] = { 0 };
									ULONG uReturnLen = 0;
									POBJECT_NAME_INFORMATION pNameInfo = (POBJECT_NAME_INFORMATION)strrRootPath;
									if (pCreateInfo->RootObject != NULL)
									{
										ObQueryNameString(pCreateInfo->RootObject, pNameInfo, sizeof(strrRootPath), &uReturnLen);
									}
									DbgPrint("RegFilter ProcessName = %s,CreateKeyEx:%wZ\\%wZ\n", pProcessName, &(pNameInfo->Name), pCreateInfo->CompleteName);

								}
								break;
	}
	}
	return STATUS_SUCCESS;
}

BOOLEAN IsAbsolute(PREG_CREATE_KEY_INFORMATION pCreateInfo)
{
	BOOLEAN bAbsolute = FALSE;
	do
	{
		if (pCreateInfo == NULL)
		{
			break;
		}
		if (!pCreateInfo->CompleteName || !pCreateInfo->CompleteName->Buffer || !pCreateInfo->CompleteName->Length)
		{
			break;
		}
		if (pCreateInfo->CompleteName->Buffer[0] != L'\\')
		{
			/*相对路径*/
			break;
		}
		/*绝对路径*/
		bAbsolute = TRUE;
	} while (FALSE);
	return bAbsolute;
}

VOID CreateThreadNotify(
	IN HANDLE  ProcessId,
	IN HANDLE  ThreadId,
	IN BOOLEAN  Create
	)
{
	if (Create)
	{
		DbgPrint("进程:%d->线程:%d创建\n", ProcessId, ThreadId);
	}
	else
	{
		DbgPrint("进程:%d->线程:%d销毁\n", ProcessId, ThreadId);
	}
}