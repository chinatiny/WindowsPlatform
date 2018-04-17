#include <ntddk.h>

/*函数声明*/
NTSTATUS CommonProc(PDEVICE_OBJECT objDeivce, PIRP pIrp);
NTSTATUS ReadProc(PDEVICE_OBJECT objDeivce, PIRP pIrp);
NTSTATUS CreateProc(PDEVICE_OBJECT objDeivce, PIRP pIrp);
NTSTATUS WriteProc(PDEVICE_OBJECT objDeivce, PIRP pIrp);

VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	UNREFERENCED_PARAMETER(pDriver);
	UNICODE_STRING pSymbolLinkName =
		RTL_CONSTANT_STRING(L"\\DosDevices\\15PBHello");
	IoDeleteSymbolicLink(&pSymbolLinkName);
	IoDeleteDevice(pDriver->DeviceObject);


	KdPrint(("Bye Bye\n"));
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	DbgBreakPoint();
	NTSTATUS status = 0;
	UNICODE_STRING pDeviceName =
		RTL_CONSTANT_STRING(L"\\Device\\Hello15PB");
	UNICODE_STRING pSymbolLinkName =
		RTL_CONSTANT_STRING(L"\\DosDevices\\15PBHello");
	PDEVICE_OBJECT pDevice = NULL;
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
	if (NT_SUCCESS(status) == FALSE)
	{
		return status;
	}
	pDevice->Flags |= DO_BUFFERED_IO;
	//pDevice->Flags |= DO_DIRECT_IO;
	//2 创建一个符号链接
	IoCreateSymbolicLink(&pSymbolLinkName, &pDeviceName);

	//3 填写IRP处理函数
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		pDriver->MajorFunction[i] = CommonProc;
	}

	pDriver->DriverUnload = DriverUnload;

	return STATUS_SUCCESS;
}

//读取处理函数
NTSTATUS ReadProc(PDEVICE_OBJECT objDeivce, PIRP pIrp) 
{
	UNREFERENCED_PARAMETER(objDeivce);
	//获取io缓冲区
	UCHAR * pIOBuff = NULL;
	if (pIrp->AssociatedIrp.SystemBuffer != NULL)
	{
		pIOBuff = pIrp->AssociatedIrp.SystemBuffer;
	}
	else if (pIrp->MdlAddress != NULL)
	{
		pIOBuff = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
	}
	ULONG uThreadID = (ULONG)PsGetCurrentThreadId();
	DbgPrint("threadid:%d\n", uThreadID);

	//设置返回数据
	wchar_t szMsg[] = L"this is a ring 0 msg";
	memcpy(pIOBuff, szMsg, wcslen(szMsg) * sizeof(wchar_t)+sizeof(wchar_t));
	// 设置IRP完成状态
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	// 设置IRP操作了多少字节
	pIrp->IoStatus.Information = wcslen(szMsg) * sizeof(wchar_t)+sizeof(wchar_t);
	// 处理IRP
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
//创建处理函数
NTSTATUS CreateProc(PDEVICE_OBJECT objDeivce, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(objDeivce);

	// 设置IRP完成状态
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	// 设置IRP操作了多少字节
	pIrp->IoStatus.Information = 0;
	// 处理IRP
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
//写入处理函数
NTSTATUS WriteProc(PDEVICE_OBJECT objDeivce, PIRP pIrp)
{
	UNREFERENCED_PARAMETER(objDeivce);
	//1 获得缓冲区的通用处理
	UCHAR * pIOBuff = NULL;
	if (pIrp->AssociatedIrp.SystemBuffer != NULL)
	{
		pIOBuff = pIrp->AssociatedIrp.SystemBuffer;
	}
	else if (pIrp->MdlAddress != NULL)
	{
		pIOBuff = MmGetSystemAddressForMdlSafe(pIrp->MdlAddress, NormalPagePriority);
	}
	//2 获得缓冲区有多大
	PIO_STACK_LOCATION pStack = IoGetCurrentIrpStackLocation(pIrp);
	pStack->Parameters.Write.Length;


	KdPrint(("%s\n", pIOBuff));

	// 设置IRP完成状态
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	// 设置IRP操作了多少字节
	pIrp->IoStatus.Information = 5;
	// 处理IRP
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

//写入处理函数
NTSTATUS CommonProc(PDEVICE_OBJECT objDeivce, PIRP pIrp)
{
	//1 获得了IRP栈
	PIO_STACK_LOCATION pStack = IoGetCurrentIrpStackLocation(pIrp);
	pStack->Parameters.Write.Length;

	switch (pStack->MajorFunction)
	{
	case IRP_MJ_READ:
		ReadProc(objDeivce, pIrp);
		break;
	case IRP_MJ_WRITE:
		WriteProc(objDeivce, pIrp);
		break;
	default:
		// 设置IRP完成状态
		pIrp->IoStatus.Status = STATUS_SUCCESS;
		// 设置IRP操作了多少字节
		pIrp->IoStatus.Information = 5;
		// 处理IRP
		IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	}
	return STATUS_SUCCESS;

}