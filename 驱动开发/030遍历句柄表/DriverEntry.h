#pragma  once
#pragma warning(disable:4189 4100 4201)
#include <Ntifs.h>

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO
{
	USHORT UniqueProcessId;
	USHORT CreatorBackTraceIndex;
	UCHAR ObjectTypeIndex;
	UCHAR HandleAttributes;
	USHORT HandleValue;
	PVOID Object;
	ULONG GrantedAccess;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

typedef struct _HANDLE_TABLE_ENTRY
{
	union
	{
		PVOID Object;
		UINT32 ObAttributes;
		struct _HANDLE_TABLE_ENTRY *InfoTable;
		UINT32 Value;
	};
	union
	{
		UINT32 GrantedAccess;
		struct
		{
			USHORT GrantedAccessIndex;
			USHORT CreatorBackTraceIndex;
		};
		INT32 NextFreeTableEntry;
	};
} HANDLE_TABLE_ENTRY, *PHANDLE_TABLE_ENTRY;

typedef struct _HANDLE_TRACE_DB_ENTRY
{
	CLIENT_ID ClientId;
	PVOID Handle;
	ULONG Type;
	VOID * StackTrace[16];
} HANDLE_TRACE_DB_ENTRY, *PHANDLE_TRACE_DB_ENTRY;

typedef struct _HANDLE_TRACE_DEBUG_INFO
{
	LONG RefCount;
	ULONG TableSize;
	ULONG BitMaskFlags;
	FAST_MUTEX CloseCompactionLock;
	ULONG CurrentStackIndex;
	HANDLE_TRACE_DB_ENTRY TraceDb[1];
} HANDLE_TRACE_DEBUG_INFO, *PHANDLE_TRACE_DEBUG_INFO;


typedef struct _HANDLE_TABLE {
	ULONG TableCode;
	PEPROCESS QuotaProcess;
	PVOID UniqueProcessId;
	EX_PUSH_LOCK HandleLock;
	LIST_ENTRY HandleTableList;
	EX_PUSH_LOCK HandleContentionEvent;
	PHANDLE_TRACE_DEBUG_INFO DebugInfo;
	LONG ExtraInfoPages;
	ULONG Flags;
	ULONG FirstFreeHandle;
	PHANDLE_TABLE_ENTRY LastFreeHandleEntry;
	LONG HandleCount;
	ULONG NextHandleNeedingPool;
} HANDLE_TABLE, *PHANDLE_TABLE;



typedef struct _OBJECT_CREATE_INFORMATION
{
		ULONG Attributes; //+0x000
		PVOID RootDirectory; //+0x004
		CHAR ProbeMode; //+0x008
		ULONG PagedPoolCharge; //+0x00c
		ULONG NonPagedPoolCharge; //+0x010
		ULONG SecurityDescriptorCharge; //+0x014
		PVOID SecurityDescriptor; //+0x018
		PSECURITY_QUALITY_OF_SERVICE SecurityQos; //+0x01c
		SECURITY_QUALITY_OF_SERVICE SecurityQualityOfService; //+0x020
}OBJECT_CREATE_INFORMATION, *POBJECT_CREATE_INFORMATION;

typedef struct __OBJECT_HEADER
{
	ULONG PointerCount; //+0x000
	union {
		ULONG HandleCount; //+0x004
		PVOID NextToFree; //+0x004
	};
	EX_PUSH_LOCK Lock; //+0x008
	UCHAR TypeIndex; //+0x00c
	UCHAR TraceFlags; //+0x00d
	UCHAR InfoMask; //+0x00e
	UCHAR Flags; //+0x00f
	union {
		POBJECT_CREATE_INFORMATION ObjectCreateInfo; //+0x010
		PVOID QuotaBlockCharged; //+0x010
	};
	PVOID SecurityDescriptor; //+0x014
	QUAD Body; //+0x018
}OBJECT_HEADER, *POBJECT_HEADER;

#define OBJECT_TO_OBJECT_HEADER(o)   CONTAINING_RECORD((o), OBJECT_HEADER, Body)
