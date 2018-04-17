/*
* 完成端口分为两部分
* 第一部分:
*  由线程waitClientAccept负责, 主要负责两个任务:
*  1. 接受客户端接入
*  2. 为客户端投递接受数据的IO任务到完成端口中
*
* 第二部分:
*  由线程waitNetworkIOProc负责, 主要负责两个任务:
*  1. 调用GetQueuedCompletionStatus等待第一部分第2步投递的IO任务完成.
*  2. 根据已经完成的IO任务的类型来处理该任务.
*
* 在main函数中, 是在做事情准备,这些准备包括:
*  1. 创建服务端套接字, 如果没有服务端套接字,第一部分的第一步无法完成接收客户端套接字.
*  2. 创建完成端口
*  3. 为完成端口创建工作线程:waitNetworkIOProc
*  4. 为等待客户端连接创建线程:waitClientAccept
*/

#include "NetWorkOverlappedRecv.h"
#include <process.h>
#include <stdio.h>


// 扩展OVERLAPPED结构体, 用于保存WSARecv投递的IO任务的信息
//class NetWorkOverlappedRecv;

SOCKET g_hServer;
HANDLE g_hIocp;

// 等待客户端连接的回调函数
unsigned int CALLBACK waitClientAccept(void *pArg);


// 等待事件的线程回调函数
unsigned int CALLBACK waitNetworkIOProc(void *pArg);


int main()
{
	// 1. 初始化套接字
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);


	//1. 创建服务端的流式TCP套接字
	g_hServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


	//2. 绑定套接字
	sockaddr_in serverAddr = { 0 };
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons((short)1234);
	InetPtonA(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // 将字符串的IP转换成整形

	// 2.1 绑定套接字到地址和端口上
	bind(g_hServer, (sockaddr*)&serverAddr, sizeof(serverAddr));


	// 3. 监听套接字
	listen(g_hServer, SOMAXCONN);


	// 4. 创建完成端口
	SYSTEM_INFO si = { sizeof(SYSTEM_INFO) };
	GetSystemInfo(&si);
	g_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE,
		NULL,
		NULL,
		si.dwNumberOfProcessors);

	// 5. 将套接字和完成端口进行绑定
	CreateIoCompletionPort((HANDLE)g_hServer,
		g_hIocp,
		0,
		si.dwNumberOfProcessors
		);


	// 6. 创建完成端口的工作线程
	for (unsigned int i = 0; i < si.dwNumberOfProcessors * 2; ++i) {

		_beginthreadex(0, 0, waitNetworkIOProc, 0, 0, 0);
	}



	// 创建等待客户端接入的线程
	HANDLE hThread = (HANDLE)_beginthreadex(0, 0, waitClientAccept, 0, 0, 0);

	WaitForSingleObject(hThread, -1);
	return 0;
}


// 等待客户端连接的回调函数
unsigned int CALLBACK waitClientAccept(void *pArg)
{

	SOCKET          hClient = NULL;
	sockaddr_in     clientAddr = { 0 };
	int             nSizeOfAddr = sizeof(clientAddr);

	while (true) {

		// 1. 接收客户端的连接
		hClient = accept(g_hServer, (sockaddr*)&clientAddr, &nSizeOfAddr);

		// 1.1 将客户端套接字和完成端口进行绑定在一起
		CreateIoCompletionPort((HANDLE)hClient,
			g_hIocp,
			0,
			0);

		//2 为新接入的客户端投递异步接收任务(这个任务将会由完成端口的等待线程去等待)

		//2.1 配置OVERLAPPED结构体
		NetWorkOverlappedRecv *pOver = new NetWorkOverlappedRecv;

		pOver->m_hSocket = hClient; // 保存新客户端套接字
		memcpy(&pOver->m_sockAddress, &clientAddr, sizeof(clientAddr)); // 保存新客户端地址

		pOver->m_buff.len = 20;     // 保存IO任务的字节数
		pOver->m_buff.buf = new CHAR[20]; // 申请控件,用于保存接收到的数据
		pOver->m_netWorkEvent = e_recv; // 此次IO任务的网络活动事件代码


		// 2.2 投递IO任务
		int     nRet = 0;
		DWORD   dwRecvSize = 0;
		DWORD   dwFlag = 0;

		WSARecv(hClient,           // 客户端套接字
			&pOver->m_buff,    // 用户保存接收到的数据
			1,                 // 接收到的字节数
			&dwRecvSize,       // 实际接收到的字节数(虽然在这里无效,但还是需要传入)
			&dwFlag,          // 标志
			pOver,             // 重叠IO结构
			NULL                // 完成函数(不使用)
			);



		// IP地址转字符串
		char szIpAddress[20];
		InetNtopA(AF_INET, &clientAddr.sin_addr, szIpAddress, 20);
		printf("IP:[%s] 上线\n", szIpAddress);

	}

}



// 等待IO任务完成的函数
unsigned int CALLBACK waitNetworkIOProc(void *pArg)
{
	DWORD   dwSize = 0;
	DWORD   dwIoKey = 0;
	BOOL    bRet = FALSE;
	NetWorkOverlappedRecv* pOver = NULL;

	while (true)
	{
		bRet = GetQueuedCompletionStatus(g_hIocp, // 完成端口句柄
			&dwSize,// IO任务完成的字节数
			&dwIoKey,// 完成键(在这里并没有使用到)
			(OVERLAPPED**)&pOver,// 重叠IO结构,保存的是我们已经扩展的重叠IO结构
			-1
			);
		if (bRet == FALSE)
			continue;

		// 这个事件是一个自定义的枚举类型
		// 这个枚举类型的值在投递IO任务的时候进行初始化的
		// 比如,调用WSARecv的时候, 这个值就被以e_recv来赋值
		switch (pOver->m_netWorkEvent) {

		case e_recv:  // 
		{
						  // IP地址转字符串
						  char szIpAddress[20];
						  InetNtopA(AF_INET, &pOver->m_sockAddress.sin_addr, szIpAddress, 20);

						  printf("IP[%s] : [%s]\n", szIpAddress, pOver->m_buff.buf);

						  char* pSend = { "数据已经收到" };
						  send(pOver->m_hSocket, pSend, strlen(pSend) + 1, 0);



						  // 继续投递IO任务, 如果不投递, 则下一次客户端发送数据过来时,
						  // 完成端口会因为内部已经没有IO任务进而不会做出任何响应,服务端
						  // 就不会再接收数据了, 至于这次应该接收多少字节, 可以根据客户端
						  // 发过来的请求类型来进行动态设置.
						  DWORD dwRecvSize = 0;
						  DWORD dwFlag = 0;
						  WSARecv(pOver->m_hSocket,
							  &pOver->m_buff,
							  pOver->m_buff.len,
							  &dwRecvSize,
							  &dwFlag,
							  pOver,
							  NULL
							  );
		}
			break;
		}

	}
}
