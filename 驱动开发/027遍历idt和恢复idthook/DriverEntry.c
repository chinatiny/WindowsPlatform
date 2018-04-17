#pragma warning(disable:4101 4100 4055 4152 4214)

#include <ntddk.h>

/*
*IDT寄存器结构
*IDTR寄存器共有48位，高32位是IDT表的基地址，低16位是IDT表的长度（Limit)
*IDT表示一张位于物理内存中的线性表，共有256个表项。
*/
typedef struct _IDT_INFO{
	UINT16 uIdtLimit;
	UINT16 uLowIdtBase;
	UINT16 uHighIdBase;
}IDT_INFO, *PIDT_INFO;
/* ----------------选择子-----------------------
|   1   |     0    |  字节
|7654321076543 2 10|  比特
|-------------|-|--|  占位
|    INDEX    |T|R |  含义
|             |I|P |
|             | |L |
*Index : 在GDT数组或LDT数组的索引号
*TI:Table Indicator ,这个值为0表示查找GDT,1表示查找LDT
*RPL:请求特权级，以什么样的权限去访问段。
*/
typedef struct _SELECTOR{
	int PRL : 2;
	int TI : 1;
	int Index : 13;
}SELECTOR, *PSELECTOR;
/*----------------------段描述符结构--------------------
|   7    |     6       |     5     |   4    |   3    |   2    |   1    |   0    |  字节
|76543210|7 6 5 4 3210 |7 65 4 3210|76543210|76543210|76543210|76543210|76543210|  比特
|--------|-|-|-|-|---- |-|--|-|----|--------|--------|--------|--------|--------|  占位
|  BASE  |G|D|0|A|LIMIT|P|D |S|TYPE|<------- BASE 23-0 ------>|<-- LIMIT 15-0 ->|  含义
|  31-24 | |/| |V|19-16| |P |
|B| |L|     | |L |

BASE: 段基址，由上图中的两部分(BASE 31-24 和 BSE23-0)组成
G：LIMIT的单位，该位 0 表示单位是字节，1表示单位是 4KB
D/B: 该位为 0 表示这是一个 16 位的段，1 表示这是一个 32 位段
AVL: 该位是用户位，可以被用户自由使用
LIMIT: 段的界限，单位由 G 位决定。数值上（经过单位换算后的值）等于段的长度（字节）- 1。
P: 段存在位，该位为 0 表示该段不存在，为 1 表示存在。
DPL：段权限
S: 该位为 1 表示这是一个数据段或者代码段。为 0 表示这是一个系统段（比如调用门，中断门等）
TYPE: 根据 S 位的结果，再次对段类型进行细分。
*/
typedef struct _GDT{
	INT64 Limit0_15 : 16;
	INT64 base0_23 : 24;
	INT64 TYPE : 4;
	INT64 S : 1;
	INT64 DPL : 2;
	INT64 P : 1;
	INT64 Limit16_19 : 4;
	INT64 AVL : 1;
	INT64 O : 1;
	INT64 D_B : 1;
	INT64 G : 1;
	INT64 Base24_31 : 8;
}GDT, *PGDT;//830089f700000068
/*------------------IDT表----------
|   7    |     6       |     5     |   4    |   3    |   2    |   1    |   0    |  字节
|76543210|76543210|7 65 43210|76543210 |76543210|76543210|76543210     |76543210|  比特
|-----------------|-|---|----|-------- |-----------------|----------------------|  占位
|  uOffsetHigh    |P|DPL|type|reserve_1|    selector     | uOffsetLow       |  含义
type 用来区分类型;中断恩的类型是0b00101(b代表的二进制数），中断门的类型是0b0D110,其中D为用来
表示描述的是16位们（0）还是32位门（1），陷阱门的类型是0b0D111.
*0x05 : 32 位的任务门
*0x06 :　16位的中断门
*0x07 :　16位的陷阱门
*0x0E :　32位的中断门
*0x0F : 32位的陷阱门
*/
typedef struct  _IDT{
	UINT16 uOffsetLow;  //处理程序低地址便宜
	UINT16 selector;    //段选择子
	UINT8 reserve_1;    //保留
	UINT8 type : 5; //中断类型
	UINT8 DPL : 2;  //特权级
	UINT8 P : 1;  //如未使用中断可置为0
	INT16 uOffsetHigh;     //处理程序高地址偏移
}IDT, *PIDT;

#define MAKE_LONG(Low,High) ((UINT32) (((UINT16)(Low))|((UINT32)((UINT16)(High)))<<16) )


VOID DriverUnload(PDRIVER_OBJECT objDriver)
{
	//避免编译器报为引用参数的警告
	UNREFERENCED_PARAMETER(objDriver);
	DbgPrint("My Driver is unloading ...\n");
}
NTSTATUS DriverEntry(
	PDRIVER_OBJECT objDriver,
	PUNICODE_STRING strRegPath
	)
{
	UNREFERENCED_PARAMETER(strRegPath);
	objDriver->DriverUnload = DriverUnload;
	DbgPrint("My first Driver\n");
	KdBreakPoint();
	IDT_INFO stcIdtInfo = { 0 };
	PIDT pIdt = NULL;
	_asm sidt stcIdtInfo;

	pIdt = (PIDT)MAKE_LONG(
		stcIdtInfo.uLowIdtBase, stcIdtInfo.uHighIdBase);
	for (UINT32 uCount = 0; uCount <= 256; uCount++)
	{
		//需要对16任务门进行特殊处理，具体参见软件调试275页
		//简单流程：TSS段选择子，GDT表，TSS段分析
		PIDT  pIndex = &pIdt[uCount];
		UINT32 uAddr = MAKE_LONG(pIndex->uOffsetLow, pIndex->uOffsetHigh);
		DbgPrint("Interrput %d: ISR 0x%08X\n", uCount, uAddr);
	}
	return STATUS_SUCCESS;
}