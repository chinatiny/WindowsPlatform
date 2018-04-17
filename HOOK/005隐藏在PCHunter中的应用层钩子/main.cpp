#include <windows.h>
#include <tchar.h>
#include "UnDocoumentApi.h"

typedef struct _PROCESS_BASIC_INFORMATION
{
	NTSTATUS ExitStatus;
	PVOID PebBaseAddress;
	ULONG_PTR AffinityMask;
	LONG BasePriority;
	ULONG_PTR UniqueProcessId;
	ULONG_PTR InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION;
typedef PROCESS_BASIC_INFORMATION *PPROCESS_BASIC_INFORMATION;


void MakePePacked(HANDLE hProcess, PBYTE pImageBuff)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pImageBuff;
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(pImageBuff + pDosHeader->e_lfanew);
	DWORD dwOld = 0;
	VirtualProtectEx(hProcess, pNtHeader, sizeof(PIMAGE_NT_HEADERS), PAGE_EXECUTE_READWRITE, &dwOld);
	pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = 1;
	pNtHeader->OptionalHeader.AddressOfEntryPoint = 0;
	VirtualProtectEx(hProcess, pNtHeader, sizeof(PIMAGE_NT_HEADERS), dwOld, &dwOld);
}


typedef NTSTATUS(WINAPI *PfnZwQueryInformationProcess)(HANDLE ProcessHandle, 
	ULONG ProcessInformationClass,
	PVOID ProcessInformation, 
	ULONG ProcessInformationLength, 
	PULONG ReturnLength);


int main(int argc, char* argv[])
{
	DWORD dwProcessId = GetCurrentProcessId();
	NTSTATUS Status;
	HANDLE hProcess = NULL;
	PfnZwQueryInformationProcess fnZwQueryInformationProcess;
	PROCESS_BASIC_INFORMATION ProcessInfo;
	PPEB    pPeb;
	fnZwQueryInformationProcess = (PfnZwQueryInformationProcess)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "ZwQueryInformationProcess");
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, dwProcessId);


	Status = fnZwQueryInformationProcess(hProcess, 0, &ProcessInfo, sizeof(ProcessInfo), NULL);
	if (NT_SUCCESS(Status))
	{
		pPeb = (PPEB)ProcessInfo.PebBaseAddress;

		for (PLIST_ENTRY pListEntry = pPeb->Ldr->InLoadOrderModuleList.Flink; pListEntry != &pPeb->Ldr->InLoadOrderModuleList; pListEntry = pListEntry->Flink)
		{
			PLDR_DATA_TABLE_ENTRY pEntry = CONTAINING_RECORD(pListEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
			_tprintf(_T("%s\n"), pEntry->FullDllName.Buffer);
			MakePePacked(hProcess, (PBYTE)pEntry->DllBase);
		}
	}
	CloseHandle(hProcess);

	while (true)
	{

	}

	return 0;
}