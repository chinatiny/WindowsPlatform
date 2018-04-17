#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "IPHLPAPI.lib")
#include <iphlpapi.h>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
/*
使用 GetIfTable2(), GetIfEntry2() 等函数需包含头文件 iphlpapi.h 和 iphlpapi.lib
iphlpapi.h 需要包含头文件 ws2tcpip.h

注意 ws2tcpip.h 与 windows.h 的包含顺序
须先包含 ws2tcpip.h 在包含 windows.h 否则会产生错误
*/

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
using namespace std;

ULONG64 GetSpeedDifference(ULONG64 num1, ULONG64 num2) //网速计算函数
{
	//由于InOctets字段是32bit整数，增至0xFFFFFFFF后会掉头到0，所以要注意减法运算。
	if (num1 >= num2)
	{
		return num1 - num2;
	}
	else
	{
		return (0xFFFFFFFF - num2) + num1;
	}
}

/*
首先使用 GetIfTable2() 获取所有的网络连接信息, 网络连接 != 网卡, 一个硬件网卡可有多个网络连接
GetIfTable2() 将会填充一个 所有网络连接的数组 PMIB_IF_TABLE2 , 的第一个字段保存所有网络连接个数,第二个字段保存网络连接数组,数组使用 PMIB_IF_ROW2 结构体保存每个网络连接信息,
GetIfTable2() 将会填充网络连接的 InterfaceLuid, InterfaceIndex, InterfaceAndOperStatusFlags, OperStatus 等字段
通过 InterfaceAndOperStatusFlags, OperStatus 判断网络连接是否为正在使用的网络, 再通过 InterfaceLuid, InterfaceIndex, 信息调用 GetIfEntry2() 函数获取该连接的具体详细信息
GetIfEntry2() 将会获取该连接的累计下载总量与累计上传总量, 通过计算:
(上一时刻该连接的 累计下载/上传量 - 当前时刻 累计下载/上传量) / 时间 = 可得出瞬时 下载/上传网速
*/
int main(void)
{
	PMIB_IF_TABLE2 m_pIfTable;
	PMIB_IF_ROW2 m_pIfRow;
	ULONG64 outSpeed, inSpeed, NowInOctets, NowOutOctets;
	ULONG64 PreInOctets = 0, PreOutOctets = 0;

	while (true)
	{
		// 获取网卡列表
		if (GetIfTable2(&m_pIfTable) == NO_ERROR)
			/*
			GetIfTable2() 函数用来填充一个 PMIB_IF_TABLE2 结构体,如果成功将会返回 NO_ERROR
			GetIfTable2() 函数填充的结构体将会使用堆空间,使用完后需要使用 FreeMibTable() 手动释放结构体


			PMIB_IF_TABLE2 存储得是本机所有网络连接信息:

			typedef struct _MIB_IF_TABLE2
			{
			ULONG NumEntries; //本机存在的所有网络连接数量
			MIB_IF_ROW2 Table[ANY_SIZE]; //一个PMIB_IF_ROW2结构体链表,PMIB_IF_ROW2结构体用来存储网络连接具体信息
			} MIB_IF_TABLE2, *PMIB_IF_TABLE2;


			MIB_IF_ROW2 存储具体网络连接的具体信息:

			typedef struct _MIB_IF_ROW2
			{
			//
			// Key structure.  Sorted by preference.
			//
			NET_LUID InterfaceLuid;     //网络接口的本地唯一标识符(LUID)
			NET_IFINDEX InterfaceIndex;     //标识网络接口的索引,如果网络适配器被禁用,在重新启用时此索引值可能会更改

			//
			// Read-Only fields.
			//
			GUID InterfaceGuid;
			WCHAR Alias[IF_MAX_STRING_SIZE + 1];
			WCHAR Description[IF_MAX_STRING_SIZE + 1];
			ULONG PhysicalAddressLength;
			UCHAR PhysicalAddress[IF_MAX_PHYS_ADDRESS_LENGTH];
			UCHAR PermanentPhysicalAddress[IF_MAX_PHYS_ADDRESS_LENGTH];

			ULONG Mtu;
			IFTYPE Type;                // Interface Type.
			TUNNEL_TYPE TunnelType;     // Tunnel Type, if Type = IF_TUNNEL.
			NDIS_MEDIUM MediaType;
			NDIS_PHYSICAL_MEDIUM PhysicalMediumType;
			NET_IF_ACCESS_TYPE AccessType;
			NET_IF_DIRECTION_TYPE DirectionType;

			struct
			{
			BOOLEAN HardwareInterface : 1;      //该连接用于硬件接口,否则为用于虚拟网卡
			BOOLEAN FilterInterface : 1;
			BOOLEAN ConnectorPresent : 1;
			BOOLEAN NotAuthenticated : 1;
			BOOLEAN NotMediaConnected : 1;
			BOOLEAN Paused : 1;
			BOOLEAN LowPower : 1;
			BOOLEAN EndPointInterface : 1;
			} InterfaceAndOperStatusFlags;      //该连接的一些标志位

			IF_OPER_STATUS OperStatus;      //连接的具体状态信息,该字段为枚举类型,枚举值如下

			//  typedef enum
			//  {
			//       IfOperStatusUp = 1,     //表示该连接已开启
			//       IfOperStatusDown,       //表示该连接关闭
			//       IfOperStatusTesting,
			//       IfOperStatusUnknown,
			//       IfOperStatusDormant,
			//       IfOperStatusNotPresent,
			//       IfOperStatusLowerLayerDown
			//  } IF_OPER_STATUS;


			NET_IF_ADMIN_STATUS AdminStatus;
			NET_IF_MEDIA_CONNECT_STATE MediaConnectState;
			NET_IF_NETWORK_GUID NetworkGuid;
			NET_IF_CONNECTION_TYPE ConnectionType;

			//
			// Statistics.
			//
			ULONG64 TransmitLinkSpeed;
			ULONG64 ReceiveLinkSpeed;

			ULONG64 InOctets;       //该网络接口的累计下载量
			ULONG64 InUcastPkts;
			ULONG64 InNUcastPkts;
			ULONG64 InDiscards;
			ULONG64 InErrors;
			ULONG64 InUnknownProtos;
			ULONG64 InUcastOctets;
			ULONG64 InMulticastOctets;
			ULONG64 InBroadcastOctets;
			ULONG64 OutOctets;          //该网络接口的累计发送量
			ULONG64 OutUcastPkts;
			ULONG64 OutNUcastPkts;
			ULONG64 OutDiscards;
			ULONG64 OutErrors;
			ULONG64 OutUcastOctets;
			ULONG64 OutMulticastOctets;
			ULONG64 OutBroadcastOctets;
			ULONG64 OutQLen;
			} MIB_IF_ROW2, *PMIB_IF_ROW2;
			*/
		{
			for (int i = 0; i < (int)m_pIfTable->NumEntries; i++)
				/*
				遍历所有的网络连接,连接数为 MIB_IF_TABLE2 中的 NumEntries
				*/
			{
				m_pIfRow = &m_pIfTable->Table[i];   //检查 MIB_IF_TABLE2 中的 Table 数组中的第 i 个元素

				if (m_pIfRow->InterfaceAndOperStatusFlags.HardwareInterface != 1)
				{
					/*
					检查连接结构体 PMIB_IF_ROW2 中的 InterfaceAndOperStatusFlags 字段的 HardwareInterface 标志位
					HardwareInterface 为1则表示该连接是用于硬件连接,非虚拟网卡,
					*/
					continue;   //是硬件连接则继续,否则跳出本次循环检查下一连接
				}

				if ((GetIfEntry2(m_pIfRow)) != NO_ERROR)
				{
					/*
					GetIfEntry2() 用来获取该连接的具体详细信息,形参 MIB_IF_ROW2 中的 InterfaceLuid 字段或 InterfaceIndex 来标识获取哪个连接,成功获取信息后将会填充形参 MIB_IF_ROW2 类型的结构体并返回 NO_ERROR
					InterfaceLuid 或 InterfaceIndex 字段由 GetIfTable2()函数填写
					*/

					//OutputDebugString(L"GetIfEntry2错误！\n");
					continue;   //获取失败则跳出本次循环,检查下一连接
				}

				if (m_pIfRow->OperStatus != IfOperStatusUp)
				{
					/*
					检查 MIB_IF_ROW2  中的 OperStatus 字段是否为 IfOperStatusUp ,是则表示该连接正在启用
					OperStatus 字段是一个枚举类型,具体可能数据如下
					//  typedef enum
					//  {
					//       IfOperStatusUp = 1,     //表示该连接已开启
					//       IfOperStatusDown,       //表示该连接关闭
					//       IfOperStatusTesting,
					//       IfOperStatusUnknown,
					//       IfOperStatusDormant,
					//       IfOperStatusNotPresent,
					//       IfOperStatusLowerLayerDown
					//  } IF_OPER_STATUS;
					*/
					continue;   //该连接被启用则继续,未启动则跳出本次循环检查下一连接
				}

				NowInOctets = m_pIfRow->InOctets;   //获取该连接的累计下载量
				NowOutOctets = m_pIfRow->OutOctets; //获取该连接的累计上传量

				inSpeed = GetSpeedDifference(NowInOctets, PreInOctets) / 1024;  //计算网速
				outSpeed = GetSpeedDifference(NowOutOctets, PreOutOctets) / 1024;   //计算网速
				/*
				使用 (上一时刻该连接的 累计下载/上传量 - 当前时刻 累计下载/上传量) / 时间 = 可得出瞬时 下载/上传网速
				*/

				if (inSpeed >= 113072)
				{
					inSpeed = 0;
				}
				if (outSpeed >= 113072)
				{
					outSpeed = 0;
				}

				printf("上传速度： %03lldKb\t", outSpeed);
				printf("下载速度： %03lldKb\r", inSpeed);

				PreInOctets = NowInOctets;
				PreOutOctets = NowOutOctets;

				break;
			}
		}

		FreeMibTable(m_pIfTable);
		/*
		GetIfTable2() 函数会分配堆空间,使用结束后需使用 FreeMibTable() 函数手动释放堆空间
		*/

		Sleep(1000);
	}
	return 0;
}
