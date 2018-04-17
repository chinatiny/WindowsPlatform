/*
* 网络模型之消息选择模型
* 原理是:
* 将套接字和窗口绑定到一起, 当套接字出现特定事件的时候, 系统将自动发送消息到窗口.
*/


// 必须定义这个宏, 否则编译器不让使用消息选择模型
#define _WINSOCK_DEPRECATED_NO_WARNINGS



#include "resource.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <Commctrl.h>
#pragma comment(lib,"Comctl32.lib")



#define UM_NETEVENT 0x1000  // 用户自定义消息 : 网络事件的消息


class NetClient;
SOCKET              g_hServer = NULL; // 服务端句柄

// 响应对话框初始化
void                OnInitDialog(HWND hWnd);
// 响应套接字的消息
void                OnSocketMessage(HWND hWnd, WPARAM wParam, LPARAM lParam);
// 对话框回调函数
INT_PTR CALLBACK    DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);



int WINAPI WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DlgProc);
}



INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {

	case WM_INITDIALOG:
		OnInitDialog(hWnd);
		break;
	case  WM_CLOSE:
		EndDialog(hWnd, 0);
		// 清理套接字控件
		WSACleanup();
		break;

	case UM_NETEVENT: // 自定义的网络事件消息
		OnSocketMessage(hWnd, wParam, lParam);
		break;

	default: return FALSE;
		break;
	}

	return TRUE;
}

void OnInitDialog(HWND hWnd)
{
	InitCommonControls(); // 初始化通用控件

	// 初始化list控件
	HWND hList = GetDlgItem(hWnd, IDC_LIST1);
	LV_COLUMN lc = { 0 };
	lc.mask = LVCF_TEXT | LVCF_WIDTH;
	lc.pszText = L"网络消息";
	lc.cx = 480;
	ListView_InsertColumn(hList, 0, &lc);

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


	// 4. 将窗口和套接字进行关联
	WSAAsyncSelect(g_hServer,     // 要和窗口关联的套接字
		hWnd,          // 要和套接字关联的窗口
		UM_NETEVENT,   // 当套接字产生网络事件时, 窗口会接到的消息(自定义,定啥都行,但不能和其他消息重复)
		FD_ACCEPT | FD_CLOSE  // 什么样的网络事件会产生消息.
		);

	//5. 在窗口回调函数中等待网络事件的消息被触发.

}




// 处理网络事件的消息被触发
void OnSocketMessage(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// wParam 保存着产生网络事件的套接字句柄
	// lParam 
	//      低16位 : 保存网络事件码
	//      高16位 : 保存错误码

	DWORD   dwNetEventCode = LOWORD(lParam);


	switch (dwNetEventCode) {

	case FD_ACCEPT: // 产生的是连接事件,有客户端连接进来
	{
						// 将连接进来的新客户端的套接字和本窗口进行关联,
						// 并选择监视这个套接字的数据发送过来时和套接字被关闭时的网络事件
						sockaddr_in clientAdddr = { 0 };
						int nSize = sizeof(clientAdddr);
						SOCKET  hClient = accept(g_hServer, (sockaddr*)&clientAdddr, &nSize);

						WSAAsyncSelect(hClient,
							hWnd,
							UM_NETEVENT,
							FD_READ | FD_CLOSE);
	}
		break;

	case FD_READ:  // 有数据被发送过来
	{

					   SOCKET  hClientScoket = (SOCKET)wParam;

					   // 接收数据
					   char data[20];
					   recv(hClientScoket, data, sizeof(data), 0);

					   // 发送数据给客户端
					   send(hClientScoket, "收到", 5, 0);


					   // 转换成宽字符然后插入到list控件.
					   TCHAR szBuff[20];
					   MultiByteToWideChar(CP_ACP, 0, data, -1, szBuff, 20);

					   // 将数据插入到list控件
					   LVITEM li = { 0 };
					   li.mask = LVIF_TEXT;
					   li.pszText = szBuff;
					   ListView_InsertItem(GetDlgItem(hWnd, IDC_LIST1), &li);
	}
		break;


	case FD_CLOSE: // 有套接字被关闭
	{
					   // 关闭套接字
					   closesocket((SOCKET)wParam);
	}
		break;
	}
}

