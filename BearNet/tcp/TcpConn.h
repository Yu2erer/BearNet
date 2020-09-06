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
    friend class TcpServer;
public:
    TcpConn(TcpServer* tcpServer, const int fd, size_t bufferSize);
    ~TcpConn();
public:
    void ShutDown();
    void Send(uint16_t cmd);
    void Send(uint16_t cmd, const void* data, int32_t dataSize);
    template <typename... T>
    void Send(uint16_t cmd, const void* data, int32_t dataSize, T... args);
    uint64_t GetID() const { return m_id; }
    bool IsConnected() const { return m_state == kConnected; }
    bool IsDisconnected() const { return m_state == kDisconnected; }
    const std::shared_ptr<CmdCallBack> GetCmdCallBack(uint16_t cmd) const;
private:
    // just TcpServer or TcpClient use.
    void _SetConnectCallBack(const ConnectCallBack& callBack) {
        m_connectCallBack = callBack;
    }
    void _SetDisconnectCallBack(const DisconnectCallBack& callBack) {
        m_disconnectCallBack = callBack;
    }
    void _SetInnerMessageCallBack(const InnerMessageCallBack& callBack) {
        m_innerMessageCallBack = callBack;
    }
    void _SetInnerCloseCallBack(const InnerCloseCallBack& callBack) {
        m_innerCloseCallBack = callBack;
    }
    TcpServer* _GetTcpServer() const { return m_tcpServer; }
    void _ConnEstablished();
    void _ConnDestroyed();
    void _Send(const void* data, uint32_t size);
    void _Send(const std::string& message);
    void _Send(Buffer* buffer);
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
    InnerMessageCallBack m_innerMessageCallBack;
    InnerCloseCallBack m_innerCloseCallBack;
    Buffer m_recvBuf;
    Buffer m_sendBuf;

};

}

#endif