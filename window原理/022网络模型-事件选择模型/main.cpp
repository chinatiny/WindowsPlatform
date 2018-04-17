#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
#include <process.h>
#include <cstdio>

HANDLE      g_hEvent[64] = { 0 }; // 保存客户端套接字的数组
SOCKET      g_hSocket[64] = { 0 };// 保存所有的套接字(服务端保存在第0个,剩下的都保存客户端)
DWORD       g_nEventCount = 0;      // 当前客户端个数
sockaddr_in g_clientAddr[64];       // 客户端地址

// 等待网络事件的线程
unsigned int CALLBACK waitNetEvent(void *pArg);

int main()
{
	// 1. 初始化套接字
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);


	//1. 创建服务端的流式TCP套接字
	g_hSocket[0] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


	//2. 绑定套接字
	sockaddr_in serverAddr = { 0 };
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons((short)1234);
	InetPtonA(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // 将字符串的IP转换成整形
	// 2.1 绑定套接字到地址和端口上
	bind(g_hSocket[0], (sockaddr*)&serverAddr, sizeof(serverAddr));


	// 3. 监听套接字
	listen(g_hSocket[0], SOMAXCONN);


	// 4. 创建事件对象
	g_hEvent[0] = WSACreateEvent();
	g_nEventCount++;

	// 5. 将套接字和事件对象进行连接
	WSAEventSelect(g_hSocket[0],         // 要和事件连接的套接字
		g_hEvent[0],            // 要和套接字连接的事件
		FD_ACCEPT | FD_CLOSE// 注册会触发事件对象的网络事件,
		);


	// 6. 创建一个线程等待事件对象被触发
	HANDLE hThread = (HANDLE)_beginthreadex(0, 0, waitNetEvent, 0, 0, 0);

	// 等待线程退出
	WaitForSingleObject(hThread, -1);

	return 0;
}

unsigned int CALLBACK waitNetEvent(void *pArg)
{

	DWORD               dwRet = 0;
	DWORD               dwIndex = 0;
	WSANETWORKEVENTS    netEvent;
	while (true)
	{

		// 等待事件对象
		dwRet = WSAWaitForMultipleEvents(g_nEventCount,
			g_hEvent,
			FALSE,
			-1,
			TRUE
			);

		// 判断返回值是否正确
		// 只有返回值在 WSA_WAIT_EVENT_0  到 WSA_WAIT_EVENT_0 + 64 之内的值才是正确
		// 因为等待函数最多只能等待64个事件. 事件被触发,就会返回事件在数组中的序号
		if (dwRet < WSA_WAIT_EVENT_0 || dwRet >= WSA_WAIT_EVENT_0 + 64) {
			continue;
		}

		// 如果WSAWaitForMultipleEvents函数正常返回.
		// 说明它所轮训等待的事件对象数组中有一个事件对象被激活.
		// 这时, 它会将事件对象在数组中的索引返回.
		// 得到当前被触发的事件对象在数组中的序号
		dwIndex = dwRet - WSA_WAIT_EVENT_0;




		// 将当前的事件所触发的网络活动枚举出来
		// 传入要枚举的套接字
		WSAEnumNetworkEvents(g_hSocket[dwIndex],    // 事件对象对应的套接字句柄
			g_hEvent[dwIndex],     // 要进行枚举的事件对象句柄
			&netEvent                 // 枚举出的信息
			);


		if (netEvent.lNetworkEvents & FD_ACCEPT) { // 有连接请求事件被触发

			// 事件对象数组和套接字数组最大下标是64, 并且, WSAEnumNetworkEvents只能
			// 等待64个以内的事件对象. 因此, 如果超出了64个,就不能再处理了
			if (g_nEventCount < 64) {


				// 接收客户端的连接
				int nSize = sizeof(sockaddr);
				// 将客户端的SOCKET保存到数组中
				g_hSocket[g_nEventCount] = accept(g_hSocket[0],
					(sockaddr*)&g_clientAddr[g_nEventCount],
					&nSize);


				// 创建一个事件对象. 并保存到数组中
				// 新事件对象在数组中的下标和新套接字在套接字数组中的下标是相等的.
				g_hEvent[g_nEventCount] = WSACreateEvent();


				// 将新的套接字和新的事件对象进行关联
				WSAEventSelect(g_hSocket[g_nEventCount], // 新套接字
					g_hEvent[g_nEventCount],  // 新事件对象
					FD_READ | FD_CLOSE           // 注册要检测的网路事件
					);


				// IP地址转字符串
				char szIpAddress[20];
				InetNtopA(AF_INET, &g_clientAddr[g_nEventCount].sin_addr, szIpAddress, 20);
				printf("IP:[%s] 上线\n", szIpAddress);



				// 增加计数
				g_nEventCount++;
			}
		}

		// 读取事件
		if (netEvent.lNetworkEvents & FD_READ) {

			// 读取内容
			char    buff[20];
			recv(g_hSocket[dwIndex], buff, 20, 0);

			char szIpAddress[20];
			InetNtopA(AF_INET, &g_clientAddr[dwIndex].sin_addr, szIpAddress, 20);

			printf("IP[%s] : [%s]\n", szIpAddress, buff);
		}

		// 客户端关闭事件
		if (netEvent.lNetworkEvents & FD_CLOSE) {

			char szIpAddress[20];
			InetNtopA(AF_INET, &g_clientAddr[dwIndex].sin_addr, szIpAddress, 20);
			printf("IP[%s] 下线\n", szIpAddress);

			// 关闭套接字
			closesocket(g_hSocket[dwIndex]);

			// 将已经关掉的套接字数组中移除
			// 移除方法:
			// 将套接字句柄,事件对象句柄,客户端地址后续的内容往前移动
			// 最后一个(第63个)无须移动.
			if (dwIndex < 63) {


				memmove(&g_hSocket[dwIndex],
					&g_hSocket[dwIndex + 1],
					(64 - dwIndex) * sizeof(g_hSocket[0])
					);

				memmove(&g_hEvent[dwIndex],
					&g_hEvent[dwIndex + 1],
					(64 - dwIndex) * sizeof(g_hEvent[0])
					);

				memmove(&g_clientAddr[dwIndex],
					&g_clientAddr[dwIndex + 1],
					(64 - dwIndex) * sizeof(g_clientAddr[0])
					);
			}

			// 减少计数
			g_nEventCount--;
		}

	}
}