#ifndef BEARNET_TCPCONN_H
#define BEARNET_TCPCONN_H

#include "BearNet/base/Noncopyable.h"
#include "BearNet/TcpCommon.h"
#include "BearNet/Socket.h"


namespace BearNet {

class Channel;
class Poller;

class TcpConn : private Noncopyable, 
    public std::enable_shared_from_this<TcpConn> {
public:
    TcpConn(Poller* poller, Socket sock);
    ~TcpConn();
public:
    bool Send(const char *buf, uint32_t len);
    void ConnEstablished();
    void ConnDestroyed();
public:
    void SetMessageCallBack(const MessageCallBack& callBack) {
        m_messageCallBack = callBack;
    }
private:
    void _HandleRead();
private:
    Poller* m_ptrPoller;
    // 应该要自己进行关闭
    Socket m_sock;
    std::unique_ptr<Channel> m_ptrChannel;
    MessageCallBack m_messageCallBack;
}; 

}

#endif