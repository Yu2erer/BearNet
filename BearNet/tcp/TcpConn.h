#ifndef BEARNET_TCPCONN_H
#define BEARNET_TCPCONN_H

#include "BearNet/base/Noncopyable.h"
#include "BearNet/SocketUtils.h"
#include "BearNet/Buffer.h"
#include "BearNet/tcp/TcpCommon.h"

namespace BearNet {

class Channel;
class Poller;
class TcpServer;

class TcpConn : private Noncopyable, 
    public std::enable_shared_from_this<TcpConn> {
public:
    TcpConn(TcpServer* tcpServer, const int fd, size_t bufferSize);
    ~TcpConn();
public:
    void Send(const void* data, uint32_t size);
    void Send(const std::string& message);
    void Send(Buffer* buffer);
    void ShutDown();
    void ConnEstablished();
    void ConnDestroyed();
public:
    uint64_t GetID() const { return m_id; }
    bool IsConnected() const { return m_state == kConnected; }
    bool IsDisconnected() const { return m_state == kDisconnected; }
    TcpServer* GetTcpServer() const { return m_tcpServer; }
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
    void SetInnerCloseCallBack(const InnerCloseCallBack& callBack) {
        m_innerCloseCallBack = callBack;
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
    TcpServer* m_tcpServer;
    Poller* m_ptrPoller;
    ConnStatus m_state;
    // 析构的时候 应当 close
    const int m_fd;
    std::unique_ptr<Channel> m_ptrChannel;
    ConnectCallBack m_connectCallBack;
    DisconnectCallBack m_disconnectCallBack;
    MessageCallBack m_messageCallBack;
    InnerCloseCallBack m_innerCloseCallBack;
    Buffer m_recvBuf;
    Buffer m_sendBuf;

}; 

}

#endif