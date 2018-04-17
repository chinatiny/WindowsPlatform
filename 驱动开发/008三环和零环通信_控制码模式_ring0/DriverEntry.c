#include <ntddk.h>

#define DEVICE_NAME L"\\Device\\TestSys"  
//ring3用CreateFile打开设备使用"\\\\.\\TestSys"
#define LINK_NAME  L"\\??\\TestSys"  

#define OPER1 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800,METHOD_BUFFERED, FILE_ANY_ACCESS)  
#define OPER2 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801,METHOD_BUFFERED, FILE_ANY_ACCESS)  

NTSTATUS IrpCreateProc(PDEVICE_OBJECT DeviceObject, PIRP pIrp);
NTSTATUS IrpCloseProc(PDEVICE_OBJECT DeviceObject, PIRP pIrp);
NTSTATUS IrpIOCtrlProc(PDEVICE_OBJECT DeviceObject, PIRP pIrp);
void DriverUnload(PDRIVER_OBJECT pDriver);

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING ustrDevName;
	UNICODE_STRING ustrLinkName;
	PDEVICE_OBJECT pDevObj = NULL;

	__asm
	{
		int 3
			mov eax, eax
			mov eax, eax
			mov eax, eax
	}

	pDriver->DriverUnload = DriverUnload;

	RtlInitUnicodeString(&ustrDevName, DEVICE_NAME);
	status = IoCreateDevice(pDriver, 0, &ustrDevName, FILE_DEVICE_UNKNOWN, 0, FALSE, &pDevObj);
	if (status != STATUS_SUCCESS)
	{
		DbgPrint("设备对象创建失败\r\n");
		return status;
	}
	pDevObj->Flags |= DO_BUFFERED_IO;
	DbgPrint("设备对象创建成功\r\n");

	RtlInitUnicodeString(&ustrLinkName, LINK_NAME);
	status = IoCreateSymbolicLink(&ustrLinkName, &ustrDevName);
	if (status != STATUS_SUCCESS)
	{
		DbgPrint("设备符号链接创建失败\r\n");
		IoDeleteDevice(pDevObj);
		return status;
	}
	DbgPrint("设备符号链接创建成功\r\n");

	pDriver->MajorFunction[IRP_MJ_CREATE] = IrpCreateProc;
	pDriver->MajorFunction[IRP_MJ_CLOSE] = IrpCloseProc;
	pDriver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IrpIOCtrlProc;
	DbgPrint("设置派发函数成功\r\n");
	return STATUS_SUCCESS;
}

//
NTSTATUS IrpCreateProc(PDEVICE_OBJECT DeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	DbgPrint("应用层打开设备对象\r\n");
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
NTSTATUS IrpCloseProc(PDEVICE_OBJECT DeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	DbgPrint("应用层关闭设备对象\r\n");
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
//
NTSTATUS IrpIOCtrlProc(PDEVICE_OBJECT DeviceObject, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
	PIO_STACK_LOCATION pIrpStack;
	ULONG uIoControlCode;
	PVOID pIoBuffer;
	ULONG uInLength;
	ULONG uOutLength;
	ULONG uRead;
	ULONG uWrite;

	//temp var
	uRead = 0;
	uWrite = 0x12345678;

	//获取Irp数据
	pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
	//获取控制码
	uIoControlCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;
	//获取缓冲区地址
	pIoBuffer = pIrp->AssociatedIrp.SystemBuffer;
	//ring3 发送数据长度
	uInLength = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
	//ring0 发送数据长度
	uOutLength = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;

	switch (uIoControlCode)
	{
	case OPER1:
	{
				  DbgPrint("IrpIOCtrlProc OPER1\n");
				  pIrp->IoStatus.Information = 0;
				  status = STATUS_SUCCESS;
				  break;
	}
	case OPER2:
	{
				  DbgPrint("OPER2 接收字节数:%d\n", uInLength);
				  DbgPrint("OPER2 发送字节数:%d\n", uOutLength);
				  memcpy(&uRead, pIoBuffer, 4);
				  DbgPrint("OPER2, uRead:0x%X\n", uRead);
				  memcpy(pIoBuffer, &uWrite, 4);
				  pIrp->IoStatus.Information = 4;
				  status = STATUS_SUCCESS;
				  break;
	}
	}

	pIrp->IoStatus.Status = status;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return status;
}

void DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	UNICODE_STRING linkName;
	RtlInitUnicodeString(&linkName, LINK_NAME);
	IoDeleteSymbolicLink(&linkName);
	IoDeleteDevice(pDriver->DeviceObject);
	DbgPrint("驱动已经被卸载\r\n");
}