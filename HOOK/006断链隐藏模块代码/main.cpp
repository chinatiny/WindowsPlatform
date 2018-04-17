#include <windows.h>
#include <stdio.h>


typedef struct _LSA_UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
}
UNICODE_STRING, *PUNICODE_STRING;

typedef struct _PEB_LDR_DATA
{
	DWORD Length; // +0x00
	bool Initialized; // +0x04
	PVOID SsHandle; // +0x08
	LIST_ENTRY InLoadOrderModuleList; // +0x0c
	LIST_ENTRY InMemoryOrderModuleList; // +0x14
	LIST_ENTRY InInitializationOrderModuleList;// +0x1c
} PEB_LDR_DATA, *PPEB_LDR_DATA; // +0x24

typedef struct _LDR_MODULE
{
	LIST_ENTRY          InLoadOrderModuleList;
	LIST_ENTRY          InMemoryOrderModuleList;
	LIST_ENTRY          InInitializationOrderModuleList;
	void*               BaseAddress;
	void*               EntryPoint;
	ULONG               SizeOfImage;
	UNICODE_STRING   FullDllName;
	UNICODE_STRING      BaseDllName;
	ULONG               Flags;
	SHORT               LoadCount;
	SHORT               TlsIndex;
	HANDLE              SectionHandle;
	ULONG               CheckSum;
	ULONG               TimeDateStamp;
} LDR_MODULE, *PLDR_MODULE;

//ÐèÒªÒþ²ØµÄDLL¾ä±ú
void HideModule(HMODULE hMod)
{
	PLIST_ENTRY Head, Cur;
	PPEB_LDR_DATA ldr;
	PLDR_MODULE ldm;
	__asm
	{
			mov eax, fs:[0x30]
			mov ecx, [eax + 0x0c] //Ldr  
			mov ldr, ecx
	}
	Head = &(ldr->InLoadOrderModuleList);
	Cur = Head->Flink;
	do
	{
		ldm = CONTAINING_RECORD(Cur, LDR_MODULE, InLoadOrderModuleList);
		if (hMod == ldm->BaseAddress)
		{
			if (ldm->InLoadOrderModuleList.Blink&&ldm->InLoadOrderModuleList.Blink->Flink)
				ldm->InLoadOrderModuleList.Blink->Flink =ldm->InLoadOrderModuleList.Flink;

			if (ldm->InLoadOrderModuleList.Flink&&ldm->InLoadOrderModuleList.Flink->Blink)
				ldm->InLoadOrderModuleList.Flink->Blink = ldm->InLoadOrderModuleList.Blink;

			if (ldm->InInitializationOrderModuleList.Blink&&ldm->InInitializationOrderModuleList.Blink->Flink)
				ldm->InInitializationOrderModuleList.Blink->Flink = ldm->InInitializationOrderModuleList.Flink;

			if (ldm->InInitializationOrderModuleList.Flink&&ldm->InInitializationOrderModuleList.Flink->Blink)
				ldm->InInitializationOrderModuleList.Flink->Blink = ldm->InInitializationOrderModuleList.Blink;

			if (ldm->InMemoryOrderModuleList.Blink&&ldm->InMemoryOrderModuleList.Blink->Flink)
				ldm->InMemoryOrderModuleList.Blink->Flink = ldm->InMemoryOrderModuleList.Flink;

			if (ldm->InMemoryOrderModuleList.Flink&&ldm->InMemoryOrderModuleList.Flink->Blink)
				ldm->InMemoryOrderModuleList.Flink->Blink = ldm->InMemoryOrderModuleList.Blink;
			break;
		}
		Cur = Cur->Flink;
	} while (Head != Cur);
}

int main(int argc, char* argv[])
{
	HideModule(GetModuleHandleA(NULL));
	while (true)
	{
		;
	}
	system("pause");
}