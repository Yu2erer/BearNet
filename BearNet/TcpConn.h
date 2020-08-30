#ifndef BEARNET_TCPCONN_H
#define BEARNET_TCPCONN_H

#include "BearNet/base/Noncopyable.h"
#include "BearNet/TcpCommon.h"
#include "BearNet/Socket.h"
#include "BearNet/Buffer.h"

namespace BearNet {

class Channel;
class Poller;

class TcpConn : private Noncopyable, 
    public std::enable_shared_from_this<TcpConn> {
public:
    TcpConn(Poller* poller, const int fd, size_t bufferSize);
    ~TcpConn();
public:
    void Send(const std::string& message);
    void ShutDown();
    void ConnEstablished();
    void ConnDestroyed();
public:
    uint64_t GetID() const { return m_id; }
public:
    void SetMessageCallBack(const MessageCallBack& callBack) {
        m_messageCallBack = callBack;
    }
    void SetCloseCallBack(const CloseCallBack& callBack) {
        m_closeCallBack = callBack;
    }
private:
    void _HandleRead();
    void _HandleWrite();
    void _HandleClose();
    void _HandleError();
private:
    uint64_t m_id;
    Poller* m_ptrPoller;
    // 析构的时候 应当 close
    const int m_fd;
    std::unique_ptr<Channel> m_ptrChannel;
    MessageCallBack m_messageCallBack;
    CloseCallBack m_closeCallBack;
    Buffer m_recvBuf;
    Buffer m_sendBuf;
}; 

}

#endif