#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>

typedef enum NetWorkEvent
{
    e_accetp = 0 ,
    e_recv ,
    e_send
};


class NetWorkOverlappedRecv :public OVERLAPPED
{
public:
    NetWorkOverlappedRecv( );

    ~NetWorkOverlappedRecv( );

public:
    WSABUF          m_buff; // 用于保存接收到的数据
    NetWorkEvent    m_netWorkEvent; // 网络事件枚举值

    SOCKET          m_hSocket;// 发起IO任务的套接字
    sockaddr_in     m_sockAddress;// 套接字的地址
};
