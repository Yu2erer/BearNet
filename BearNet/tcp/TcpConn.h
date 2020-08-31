#ifndef BEARNET_TCPCONN_H
#define BEARNET_TCPCONN_H

#include "BearNet/base/Noncopyable.h"
#include "BearNet/SocketUtils.h"
#include "BearNet/Buffer.h"
#include "BearNet/tcp/TcpCommon.h"

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
    bool IsConnected() const { return m_state == kConnected; }
    bool IsDisconnected() const { return m_state == kDisconnected; }
public:
    void SetConnectCallBack(const ConnectCallBack& callBack) {
        m_connectCallBack = callBack;
    }
    void SetDisconnectCallBack(const DisconnectCallBack& callBack) {
        m_disconnectCallBack = callBack;
    }
    void SetMessageCallBack(const MessageCallBack& callBack) {
        m_messageCallBack = callBack;
    }
    // just TcpServer or TcpClient use.
    void SetCloseCallBack(const CloseCallBack& callBack) {
        m_closeCallBack = callBack;
    }
private:
    void _HandleRead();
    void _HandleWrite();
    void _HandleClose();
    void _HandleError();
private:
    enum ConnStatus {
        kDisconnected, kConnecting, kConnected, kDisconecting
    };
    uint64_t m_id;
    Poller* m_ptrPoller;
    ConnStatus m_state;
    // 析构的时候 应当 close
    const int m_fd;
    std::unique_ptr<Channel> m_ptrChannel;
    ConnectCallBack m_connectCallBack;
    DisconnectCallBack m_disconnectCallBack;
    MessageCallBack m_messageCallBack;
    CloseCallBack m_closeCallBack;
    Buffer m_recvBuf;
    Buffer m_sendBuf;

}; 

}

#endif