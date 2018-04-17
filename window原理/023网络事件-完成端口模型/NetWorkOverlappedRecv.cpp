#include "NetWorkOverlappedRecv.h"
#pragma comment(lib,"ws2_32.lib")


NetWorkOverlappedRecv::NetWorkOverlappedRecv( )
    :m_hSocket( ) ,
    m_sockAddress( ) ,
    m_netWorkEvent( e_recv ) // 接收数据的网路事件
{
    OVERLAPPED::hEvent = NULL;
    m_buff.buf = NULL;
}


NetWorkOverlappedRecv::~NetWorkOverlappedRecv( )
{
    if( m_buff.buf != NULL ) { // 释放缓冲区
        delete[ ] m_buff.buf;
    }
    if( m_hSocket != NULL ) { // 关闭套接字
        closesocket( m_hSocket );
    }
}
