#pragma warning(disable:4101 4100 4055 4152 4214 4127 4057)

#include <ntddk.h>  
#include <wdm.h>  

#define PROCESS_TERMINATE         0x0001    
#define PROCESS_VM_OPERATION      0x0008    
#define PROCESS_VM_READ           0x0010    
#define PROCESS_VM_WRITE          0x0020    
//#define PROCESS_DUP_HANDLE      0x0040

extern UCHAR * PsGetProcessImageFileName(__in PEPROCESS Process);
typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY    InLoadOrderLinks;
	LIST_ENTRY    InMemoryOrderLinks;
	LIST_ENTRY    InInitializationOrderLinks;
	PVOID            DllBase;
	PVOID            EntryPoint;
	ULONG            SizeOfImage;
	UNICODE_STRING    FullDllName;
	UNICODE_STRING     BaseDllName;
	ULONG            Flags;
	USHORT            LoadCount;
	USHORT            TlsIndex;
	PVOID            SectionPointer;
	ULONG            CheckSum;
	PVOID            LoadedImports;
	PVOID            EntryPointActivationContext;
	PVOID            PatchInformation;
	LIST_ENTRY    ForwarderLinks;
	LIST_ENTRY    ServiceTagLinks;
	LIST_ENTRY    StaticLinks;
	PVOID            ContextInformation;
	ULONG            OriginalBase;
	LARGE_INTEGER    LoadTime;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;


PVOID g_pRegiHandle = NULL;
OB_PREOP_CALLBACK_STATUS MyObjectPreCallback
(
__in PVOID  RegistrationContext,
__in POB_PRE_OPERATION_INFORMATION  OperationInformation
)
{
	if (OperationInformation->KernelHandle)
		return OB_PREOP_SUCCESS;
	//DbgPrint(PsGetProcessImageFileName((PEPROCESS)OperationInformation->Object));
	if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE)//这是把所有的进程Create操作都过滤了。禁止了终止、内存读写的权限，于是就实现了进程保护啦  
	{
		if ((OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_TERMINATE) == PROCESS_TERMINATE)
		{
			//Terminate the process, such as by calling the user-mode TerminateProcess routine..  
			OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_TERMINATE;
		}
		if ((OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_VM_OPERATION) == PROCESS_VM_OPERATION)
		{
			//Modify the address space of the process, such as by calling the user-mode WriteProcessMemory and VirtualProtectEx routines.  
			OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_OPERATION;
		}
		if ((OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_VM_READ) == PROCESS_VM_READ)
		{
			//Read to the address space of the process, such as by calling the user-mode ReadProcessMemory routine.  
			OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_READ;
		}
		if ((OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_VM_WRITE) == PROCESS_VM_WRITE)
		{
			//Write to the address space of the process, such as by calling the user-mode WriteProcessMemory routine.  
			OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_WRITE;
		}
		if ((OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_DUP_HANDLE) == PROCESS_DUP_HANDLE)
		{
			//Write to the address space of the process, such as by calling the user-mode DuplicateHandle routine.  
			OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_DUP_HANDLE;
		}


	}
	return OB_PREOP_SUCCESS;
}

NTSTATUS g_RegisterCallbacks = STATUS_UNSUCCESSFUL;

VOID DriverUnload(_In_ struct _DRIVER_OBJECT *DriverObject)
{
	ObUnRegisterCallbacks(g_pRegiHandle);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	OB_OPERATION_REGISTRATION oor;
	OB_CALLBACK_REGISTRATION ob;

	PLDR_DATA_TABLE_ENTRY ldr;
	ldr = (PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection;
	ldr->Flags |= 0x20;//加载驱动的时候会判断此值。必须有特殊签名才行，增加0x20即可。否则将调用失败   
	DriverObject->DriverUnload = DriverUnload;

	oor.ObjectType = PsProcessType;
	oor.Operations = OB_OPERATION_HANDLE_CREATE;
	oor.PreOperation = MyObjectPreCallback;
	oor.PostOperation = NULL;

	ob.Version = OB_FLT_REGISTRATION_VERSION;
	ob.OperationRegistrationCount = 1;
	ob.OperationRegistration = &oor;
	RtlInitUnicodeString(&ob.Altitude, L"321000");
	ob.RegistrationContext = NULL;

	g_RegisterCallbacks = ObRegisterCallbacks(&ob, &g_pRegiHandle);
	KdPrint(("%d\n", g_RegisterCallbacks));

	return  STATUS_SUCCESS;
}