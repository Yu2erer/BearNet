#ifndef BEARNET_TCPSERVER_H
#define BEARNET_TCPSERVER_H

#include <unordered_map>

#include "BearNet/tcp/TcpConn.h"
#include "BearNet/NetHandle.h"
#include "BearNet/codec/InnerCodec.h"
#include "BearNet/tcp/TcpAcceptor.h"

namespace BearNet {

class Poller;

class TcpServer : public NetHandle {
public:
    template <typename... T>
    TcpServer(Poller* poller, Codec<T...>* codec, size_t bufferSize = 10)
        : m_ptrPoller(poller),
          m_ptrCodec(std::make_shared<InnerCodec>(codec)),
          m_ip(),
          m_port(0),
          m_bufferSize(bufferSize),
          m_ptrAcceptor(new TcpAcceptor(m_ptrPoller)) {
        m_ptrAcceptor->SetNewConnectionCallBack(std::bind(&TcpServer::_NewConnection, this, std::placeholders::_1));
    }
    
    ~TcpServer();
public:
    void Start(const std::string& ip, uint16_t port);
    void Stop();
    Poller* GetPoller() const { return m_ptrPoller; }
    const std::shared_ptr<InnerCodec>& GetCodec() const { return m_ptrCodec; }
public:
    void SetConnectCallBack(const ConnectCallBack& callBack) {
        m_connectCallBack = callBack;
    }
    void SetDisconnectCallBack(const DisconnectCallBack& callBack) {
        m_disconnectCallBack = callBack;
    }
public:
    static void Send(const TcpConnPtr& conn, uint16_t cmd);
    static void Send(const TcpConnPtr& conn, uint16_t cmd, const void* data, int32_t dataSize);
    template <typename... T>
    static void Send(const TcpConnPtr& conn, uint16_t cmd, const void* data, int32_t dataSize, T... args);
private:
    void _NewConnection(int fd);
    void _DeleteConnection(const TcpConnPtr& conn);
    void _InnerMessageCallBack(const TcpConnPtr& conn, Buffer* buf);
private:
    Poller* m_ptrPoller;
    std::shared_ptr<InnerCodec> m_ptrCodec;
    std::string m_ip;
    uint16_t m_port;
    size_t m_bufferSize;
    std::unique_ptr<TcpAcceptor> m_ptrAcceptor;
    ConnectCallBack m_connectCallBack;
    DisconnectCallBack m_disconnectCallBack;
    // TcpConnPtr 引用, 移除出去时, 有可能会销毁
    std::unordered_map<uint64_t, TcpConnPtr> m_connMap;
};

template <typename... T>
void TcpServer::Send(const TcpConnPtr& conn, uint16_t cmd, const void* data, int32_t dataSize, T... args) {
    Buffer buffer;
    printf("Size: %ld\n",sizeof...(args));
    conn->_GetTcpServer()->GetCodec()->Encode(&buffer, cmd, data, dataSize, args...);
    conn->_Send(&buffer);
}

template <typename... T>
void TcpConn::Send(uint16_t cmd, const void* data, int32_t dataSize, T... args) {
    TcpServer::Send(shared_from_this(), cmd, data, dataSize, args...);
}

}

#endif