#include <ntifs.h>
VOID TimerProc(struct _DEVICE_OBJECT *DeviceObject, PVOID Context);
#pragma alloc_text(NONE_PAGE,TimerProc)//声明成函数占用非分页内存

VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	IoStopTimer(pDriver->DeviceObject);
	IoDeleteDevice(pDriver->DeviceObject);
	KdPrint(("Leave"));
}

VOID TimerProc(struct _DEVICE_OBJECT *DeviceObject, PVOID Context)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Context);
	static int a = 0;
	KdPrint(("Hello 15PB:%d\n", a));
	a++;
}

#define IOTIMER_HEAD_OFFEST        0x56

// nt!_IO_TIMER
// + 0x000 Type             : Int2B
// + 0x002 TimerFlag : Int2B
// + 0x004 TimerList : _LIST_ENTRY
// + 0x00c TimerRoutine : Ptr32     void
// + 0x010 Context : Ptr32 Void
// + 0x014 DeviceObject : Ptr32 _DEVICE_OBJECT

typedef struct __IO_TIMER
{
	USHORT Type;
	USHORT TimerFlag;
	LIST_ENTRY TimerList;
	PULONG TimerRoutine;
	ULONG Contex;
	PULONG DeviceObject;
}MIO_TIMER, *PMIO_TIMER;

//遍历io定时器
void EnumIoTimer()
{
	PLIST_ENTRY IopTimerQueueHead, pNext;
	PMIO_TIMER        pIoTimer;
	IopTimerQueueHead = *(PLIST_ENTRY*)((ULONG)IoInitializeTimer + IOTIMER_HEAD_OFFEST);
	if (!MmIsAddressValid((PVOID)IopTimerQueueHead)){ return; }
	pNext = IopTimerQueueHead->Flink;
	DbgPrint("DEVICE_OBJECT");
	while (pNext != IopTimerQueueHead)
	{
		pIoTimer = CONTAINING_RECORD(pNext, MIO_TIMER, TimerList);
		DbgPrint("type:%d  routine:0x%X\n", \
			pIoTimer->Type, pIoTimer->TimerRoutine);
		pNext = pNext->Flink;
	}
}


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	KdPrint(("Hello 15PB\n"));

	DbgBreakPoint();

	//1创建设备
	UNICODE_STRING pDeviceName =
		RTL_CONSTANT_STRING(L"\\Device\\Hello15PB");
	PDEVICE_OBJECT pDevice = NULL;
	NTSTATUS status = 0;
	//1 创建一个设备
	status = IoCreateDevice(
		pDriver,
		0,
		&pDeviceName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&pDevice
		);
	//2 初始化一个计时器
	IoInitializeTimer(pDevice, TimerProc, NULL);

	IoStartTimer(pDevice);
	pDriver->DriverUnload = DriverUnload;

	//遍历io定时器
	//EnumIoTimer();
	return STATUS_SUCCESS;
}
