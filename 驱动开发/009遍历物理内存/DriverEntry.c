#include <ntddk.h>

VOID DriverUnload(PDRIVER_OBJECT pDriver);
VOID EnumPhyMem();

NTSTATUS DriverEntry(
	PDRIVER_OBJECT pDriver,
	PUNICODE_STRING pPath
	)
{
	pPath;
	_asm int 3;
	EnumPhyMem();
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}



VOID EnumPhyMem()
{
	PPHYSICAL_MEMORY_RANGE physicalMemoryBlock = MmGetPhysicalMemoryRanges();
	if (NULL == physicalMemoryBlock)
	{
		KdPrint(("call MmGetPhysicalMemoryRanges error"));
		return;
	}

	ULONG i = 0;
	while (physicalMemoryBlock[i].NumberOfBytes.QuadPart != 0)
	{
		PHYSICAL_ADDRESS baseAddress = physicalMemoryBlock[i].BaseAddress;
		LARGE_INTEGER numberOfBytes = physicalMemoryBlock[i].NumberOfBytes;

		KdPrint(("BaseAddress: %I64x\n", baseAddress.QuadPart));
		KdPrint(("NumberOfBytes: %I64x\n", numberOfBytes.QuadPart));

		while (numberOfBytes.QuadPart > 0)
		{
			PUCHAR pMapppedBuff = (PUCHAR)MmMapIoSpace(baseAddress, PAGE_SIZE, MmNonCached);
			if (pMapppedBuff)
			{
				KdPrint(("Force READ Map %p\r\n", baseAddress.QuadPart));
				ULONG uResionID = *(PULONG)pMapppedBuff;
				if (uResionID == 0x10)
				{
					//scan_vmcs_SANDYBRIDGE(pMapppedBuff, baseAddress);
				}
				if (uResionID == 0x1)
				{
					//scan_vmcs_vmware_nested(pMapppedBuff, baseAddress);
				}
				MmUnmapIoSpace(pMapppedBuff, PAGE_SIZE);
			}
			baseAddress.QuadPart += PAGE_SIZE;
			numberOfBytes.QuadPart -= PAGE_SIZE;
		}
		i++;
	}
	ExFreePool(physicalMemoryBlock);

}



VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	pDriver;
	KdPrint(("Driver Unload"));
}