#include <ntddk.h>

typedef struct 
{
	UNICODE_STRING strName;
	LIST_ENTRY listEntry;
	ULONG uAge;
} Node;


VOID ShowNode(Node *head)
{
	PLIST_ENTRY pEntry = &head->listEntry;
	PLIST_ENTRY pHeadEntry = pEntry;
	do 
	{
		Node* pNode = CONTAINING_RECORD(pEntry, Node, listEntry);
		DbgPrint("name:%wZ, age:%d\n", &pNode->strName, pNode->uAge);
		pEntry = pEntry->Flink;
	} while (pEntry != pHeadEntry);
}

VOID DriverUnload(PDRIVER_OBJECT pDirver)
{
	UNREFERENCED_PARAMETER(pDirver);
	DbgPrint("驱动退出\n");

}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pPath)
{
	UNREFERENCED_PARAMETER(pPath);
	UNREFERENCED_PARAMETER(pDriver);
	pDriver->DriverUnload = DriverUnload;
	DbgBreakPoint();
	//
	Node node1;
	RtlInitUnicodeString(&node1.strName, L"nod1");
	node1.uAge = 12;

	//初始化链表
	InitializeListHead(&node1.listEntry);


	//头插
	Node node2;
	RtlInitUnicodeString(&node2.strName, L"nod2");
	node2.uAge = 13;
	InsertHeadList(&node1.listEntry, &node2.listEntry);
	ShowNode(&node1);


	//尾插
	Node node3;
	RtlInitUnicodeString(&node3.strName, L"nod3");
	node3.uAge = 14;
	InsertTailList(&node1.listEntry, &node3.listEntry);


	//删除头,(其实是删除是第二个)
	PLIST_ENTRY pHasDeleteEntry = RemoveHeadList(&node1.listEntry);
	Node* pHasDeletedNode = CONTAINING_RECORD(pHasDeleteEntry, Node, listEntry);
	DbgPrint("after RemoveHeadList name:%wZ, age:%d\n", &pHasDeletedNode->strName, pHasDeletedNode->uAge);


	//插入尾部
	Node node4;
	RtlInitUnicodeString(&node4.strName, L"nod4");
	node4.uAge = 15;
	InsertTailList(&node1.listEntry, &node4.listEntry);

	DbgPrint("after InsertTailList:\n");
	ShowNode(&node1);


	//删除尾
	RemoveTailList(&node1.listEntry);
	DbgPrint("after remove tail:\n");
	ShowNode(&node1);


	//删除指定节点
	DbgPrint("after remove spec node:\n");
	RemoveEntryList(node1.listEntry.Blink);
	ShowNode(&node1);


	//判断是否为空
	BOOLEAN  bRet = IsListEmpty(pHasDeleteEntry);
	DbgPrint("IsListEmpty:%d:\n", bRet);


// 	while (!IsListEmpty(&linkListHead))
// 	{
// 		PLIST_ENTRY pEntry = RemoveTailList(&linkListHead);
// 		pData = CONTAINING_RECORD(pEntry,
// 			MYDATASTRUCT,
// 			ListEntry);
// 		KdPrint(("%d\n", pData->number));
// 		ExFreePool(pData);
// 	}

	return STATUS_SUCCESS;
}