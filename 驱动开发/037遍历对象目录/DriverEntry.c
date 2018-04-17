#pragma warning(disable:4101 4100 4055 4152)

#include <ntddk.h>



#define MAX_TABLE                        37
#define MAX_OBJECT_COUNT        0x10000
typedef ULONG DEVICE_MAP;
typedef ULONG EX_PUSH_LOCK;
typedef struct _OBJECT_DIRECTORY_ENTRY{                //目录对象里的表的链表 小表  dt _OBJECT_DIRECTORY_ENTRY 8a408cc8 
	struct _OBJECT_DIRECTORY_ENTRY *ChainLink;
	PVOID        Object;
	ULONG        HashValue;
}OBJECT_DIRECTORY_ENTRY, *POBJECT_DIRECTORY_ENTRY;
typedef struct _OBJECT_DIRECTORY{                            //目录对象 大表  dt _object_directory -b 8a405ed0
	POBJECT_DIRECTORY_ENTRY        HashBuckets[MAX_TABLE];
	EX_PUSH_LOCK        Lock;
	DEVICE_MAP                DeviceMap;
	ULONG                        SessionId;
	PVOID                        NamespaceEntry;
	ULONG                        Flags;
}OBJECT_DIRECTORY, *POBJECT_DIRECTORY;
typedef struct _OBJECT_HEADER_NAME_INFO{	//查询内核对象地址 返回的结构 
	POBJECT_DIRECTORY        Directory;
	UNICODE_STRING                Name;
	ULONG                                ReferenceCount;
}OBJECT_HEADER_NAME_INFO, *POBJECT_HEADER_NAME_INFO;


typedef PVOID(*OBQUERYNAMEINFO)(IN PVOID Object);//通过内核对象地址 查询 名字
OBQUERYNAMEINFO ObQueryNameInfo;//这个函数枚举对象目录


void EnumObjectDirectory()
{
	UCHAR                                type_index;
	ULONG                                u_index;
	ULONG                                u_front, u_rear;
	UNICODE_STRING                str_func_name;
	POBJECT_DIRECTORY        ObpRootDirectoryObject;
	POBJECT_DIRECTORY_ENTRY*        queue;
	POBJECT_HEADER_NAME_INFO        pobject_name_info;//查询内核对象地址 返回的结构  	//ObQueryNameInfo是查询对象名字，当时好像只有vista以后的系统才有这个函数
	RtlInitUnicodeString(&str_func_name, L"ObQueryNameInfo");
	ObQueryNameInfo = (OBQUERYNAMEINFO)MmGetSystemRoutineAddress(&str_func_name);
	if (!MmIsAddressValid(ObQueryNameInfo))
	{
		KdPrint(("ObQueryNameInfo is null."));
		return;
	}
	ObpRootDirectoryObject = (POBJECT_DIRECTORY)0x8a205ed0;//根目录表地址  dd ObpRootDirectoryObject		//这里我直接硬编码了，实际应该通过特征码获取ObpRootDirectoryObject 的值的。注意你们用的时候一定要改，不然蓝屏
	//因为我用队列遍历这个看起来像是38叉树，所以在这里申请队列内存。可能有人问为什么不用数组呢？因为在函数中申请太大数组可能导致内核栈被破坏，所以我才这样做
	queue = (POBJECT_DIRECTORY_ENTRY*)ExAllocatePool(NonPagedPool, MAX_OBJECT_COUNT*sizeof(ULONG));
	if (!MmIsAddressValid(queue))
	{
		KdPrint(("queue is null.\n"));
		return;
	}
	u_front = u_rear = 0;
	for (u_index = 0; u_index < MAX_TABLE; u_index++)//dt _object_directory -b 8a405ed0		//先把ObpRootDirectoryObject->HashBuckets的37个元素加入队列
	{
		if (MmIsAddressValid(ObpRootDirectoryObject->HashBuckets[u_index]))
		{
			u_rear = (u_rear + 1) % MAX_OBJECT_COUNT;//计算队尾在哪  队列的第0项不用 从队列的第一项开始用起
			queue[u_rear] = (POBJECT_DIRECTORY_ENTRY)ObpRootDirectoryObject->HashBuckets[u_index];//给队尾赋值 大的37项表
			//KdPrint(("%x\n", queue[u_rear]));
		}
	}
	while (u_front != u_rear)	//当队列头追上队列尾的时候我们结束循环，说明遍历完了  队列头开始后移，追赶队列尾
	{
		u_front = (u_front + 1) % MAX_OBJECT_COUNT;
		pobject_name_info = (POBJECT_HEADER_NAME_INFO)ObQueryNameInfo(queue[u_front]->Object);//取队列中的元素，然后得到object，然后获取object的名字
		if (MmIsAddressValid(pobject_name_info))
		{
			//为了安全，我们判断下pobject_name_info是否有效再打印
			//KdPrint(("%X:::%wZ\n", queue[u_front]->Object, &pobject_name_info->Name));//内核对象地址  37项的大数据
		}
		//type_index 表示对象对应的对象类型在ObTypeIndexTable中的下标值，我这里的Directory对象类型下标值是3，所以写了硬编码，你们用的时候这里要注意，可能和我的不同，我只做演示所以没有好好判断(一般比较名字最好)。
		type_index = *(UCHAR*)((ULONG)queue[u_front]->Object - 0xc);//-0x18+c就是-c
		//进程和线程对象的地址
		if (type_index == 0x7 && type_index == 0x8)
		{
			KdPrint(("%X : index:%d\n", queue[u_front]->Object, type_index));//内核对象地址  37项的大数据
		}
		if (type_index == 0x3)//节点的内核对象地址的类型 等于3是目录对象
		{
			//这里判断对象是目录对象后，那么我们进一步获取这个目录对象下面的所有子节点加入到队列里
			for (u_index = 0; u_index < MAX_TABLE; u_index++)
			{
				if (MmIsAddressValid(((POBJECT_DIRECTORY)queue[u_front]->Object)->HashBuckets[u_index]))
				{
					u_rear = (u_rear + 1) % MAX_OBJECT_COUNT;
					queue[u_rear] = (POBJECT_DIRECTORY_ENTRY)((POBJECT_DIRECTORY)queue[u_front]->Object)->HashBuckets[u_index];//把37项表加入
				}
			}
		}
		if (queue[u_front]->ChainLink != NULL) //dt _OBJECT_DIRECTORY_ENTRY    ChainLink    Object    HashValue 
		{
			//可以理解成38叉树的一个子节点，有效的话也加入队列
			u_rear = (u_rear + 1) % MAX_OBJECT_COUNT;
			queue[u_rear] = queue[u_front]->ChainLink;//下一个节点
		}
	}
	ExFreePool(queue);
}
VOID MyUnload(PDRIVER_OBJECT        pDriverObject)
{
	KdPrint(("驱动卸载 成功\n"));
}
NTSTATUS DriverEntry(PDRIVER_OBJECT        pDriverObject, PUNICODE_STRING Reg_Path)
{
	EnumObjectDirectory();
	pDriverObject->DriverUnload = MyUnload;
	return STATUS_SUCCESS;
}

