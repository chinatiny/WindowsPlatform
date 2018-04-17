#include <ntddk.h>


VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	pDriver;
	KdPrint(("Driver Unload\n"));
}

PMDL MakeAddrWritable(ULONG ulOldAddress, ULONG ulSize, PULONG pulNewAddress)
{
	PVOID pNewAddr;
	PMDL pMdl = IoAllocateMdl((PVOID)ulOldAddress, ulSize, FALSE, TRUE, NULL);
	if (!pMdl)
		return NULL;
	PAGED_CODE();

	__try {
		MmProbeAndLockPages(pMdl, KernelMode, IoWriteAccess);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		IoFreeMdl(pMdl);
		return NULL;
	}

	if (pMdl->MdlFlags & (MDL_MAPPED_TO_SYSTEM_VA | MDL_SOURCE_IS_NONPAGED_POOL))
		pNewAddr = pMdl->MappedSystemVa;
	else                                            // Map a new VA!!!
		pNewAddr = MmMapLockedPagesSpecifyCache(pMdl, KernelMode, MmCached, NULL, FALSE, NormalPagePriority);

	if (!pNewAddr) {
		MmUnlockPages(pMdl);
		IoFreeMdl(pMdl);
		return NULL;
	}

	if (pulNewAddress)
		*pulNewAddress = (ULONG)pNewAddr;

	return pMdl;
}

NTSTATUS DriverEntry(
	PDRIVER_OBJECT pDriver,
	PUNICODE_STRING pPath
	)
{
	pPath;
	pDriver->DriverUnload = DriverUnload;

	return STATUS_SUCCESS;
}