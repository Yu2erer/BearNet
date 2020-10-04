#ifndef BEARNET_TCPCONN_H
#define BEARNET_TCPCONN_H

#include "BearNet/base/Noncopyable.h"
#include "BearNet/SocketUtils.h"
#include "BearNet/Buffer.h"
#include "BearNet/tcp/TcpCommon.h"
#include "BearNet/base/Log.h"
#include "BearNet/Channel.h"

namespace BearNet {

class Poller;
template <typename... PARAMS>
class TcpServer;

class TcpConn : private Noncopyable, 
    public std::enable_shared_from_this<TcpConn> {
public:
    template <typename... PARAMS>
    TcpConn(TcpServer<PARAMS...>* tcpServer, const int fd, size_t bufferSize)
        : m_tcpServer(tcpServer),
        m_ptrPoller(tcpServer->GetPoller()),
        m_state(kConnecting),
        m_fd(fd),
        m_ptrChannel(new Channel(m_fd, m_ptrPoller)),
        m_recvBuf(bufferSize),
        m_sendBuf(bufferSize) {

        static uint64_t id = 0;
        m_id = ++ id;

        SocketUtils::SetKeepAlive(m_fd);

        m_ptrChannel->SetReadCallBack(std::bind(&TcpConn::_HandleRead, this));
        m_ptrChannel->SetWriteCallBack(std::bind(&TcpConn::_HandleWrite, this));
        m_ptrChannel->SetCloseCallBack(std::bind(&TcpConn::_HandleClose, this));
        m_ptrChannel->SetErrorCallBack(std::bind(&TcpConn::_HandleError, this));
    
        LogTrace("TcpConn::TcpConn");
    }

    ~TcpConn();
public:
    void ShutDown();
    // template <typename... PARAMS>
    // void Send(uint16_t cmd, PARAMS... args) {
    //     TcpServer<PARAMS...>::Send(shared_from_this(), cmd, args...);
    // }
    template <typename... PARAMS>
    void Send(uint16_t cmd, const void* data, int dataSize, PARAMS... args);
    uint64_t GetID() const { return m_id; }
    bool IsConnected() const { return m_state == kConnected; }
    bool IsDisconnected() const { return m_state == kDisconnected; }
    const std::shared_ptr<void> GetCmdCallBack(uint16_t cmd) const;
public:
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
    template <typename... PARAMS>
    TcpServer<PARAMS...>* _GetTcpServer() const { 
        LogTrace("%ld", sizeof...(PARAMS));
        return static_cast<TcpServer<PARAMS...>*>(m_tcpServer);
    }
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
    void* m_tcpServer;
    Poller* m_ptrPoller;
    ConnStatus m_state;
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