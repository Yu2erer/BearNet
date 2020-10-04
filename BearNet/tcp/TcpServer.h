#ifndef BEARNET_TCPSERVER_H
#define BEARNET_TCPSERVER_H

#include <assert.h>
#include <unordered_map>

#include "BearNet/base/Log.h"
#include "BearNet/tcp/TcpConn.h"
#include "BearNet/NetHandle.h"
#include "BearNet/Codec.h"
#include "BearNet/tcp/TcpAcceptor.h"

namespace BearNet {

class Poller;

template <typename... PARAMS>
class TcpServer : public NetHandle<PARAMS...> {
public:
    TcpServer(Poller* poller, size_t bufferSize = 10)
        : m_ptrPoller(poller),
          m_ptrCodec(std::make_shared<Codec<PARAMS...>>()),
          m_ip(),
          m_port(0),
          m_bufferSize(bufferSize),
          m_ptrAcceptor(new TcpAcceptor(m_ptrPoller)) {
        m_ptrAcceptor->SetNewConnectionCallBack(std::bind(&TcpServer::_NewConnection, this, std::placeholders::_1));
    }
    
    ~TcpServer() {
        for (auto& item : m_connMap) {
            item.second->_ConnDestroyed();
            item.second.reset();
        }
        m_connMap.clear();
    }
public:
    void Start(const std::string& ip, uint16_t port) {
        m_ip = ip;
        m_port = port;
        m_ptrAcceptor->Listen(m_ip, m_port);
    }
    void Stop();
    Poller* GetPoller() const { return m_ptrPoller; }
    const std::shared_ptr<Codec<PARAMS...>>& GetCodec() const { return m_ptrCodec; }
public:
    void SetConnectCallBack(const ConnectCallBack& callBack) {
        m_connectCallBack = callBack;
    }
    void SetDisconnectCallBack(const DisconnectCallBack& callBack) {
        m_disconnectCallBack = callBack;
    }
public:
    void SetEncodeFunc(const EncodeFunc<PARAMS...>& encodeFunc) {
        GetCodec()->SetEncodeFunc(encodeFunc);
    }
    void SetDecodeFunc(const DecodeFunc<PARAMS...>& decodeFunc) {
        GetCodec()->SetDecodeFunc(decodeFunc);
    }
public:
    static void Send(const TcpConnPtr& conn, uint16_t cmd, PARAMS... args) {
        TcpServer::Send(conn, cmd, nullptr, 0, args...);
    }
    static void Send(const TcpConnPtr& conn, uint16_t cmd, const void* data, int dataSize, PARAMS... args);
private:
    void _NewConnection(int fd) {
        LogDebug("TcpServer New Connection");
        TcpConnPtr conn(new TcpConn(this, fd, m_bufferSize));
        
        // 外部回调
        conn->_SetConnectCallBack(m_connectCallBack);
        conn->_SetDisconnectCallBack(m_disconnectCallBack);

        // 内部回调
        conn->_SetInnerMessageCallBack(std::bind(&TcpServer::_InnerMessageCallBack, this, std::placeholders::_1, std::placeholders::_2));
        conn->_SetInnerCloseCallBack(std::bind(&TcpServer::_DeleteConnection, this, std::placeholders::_1));

        // 存储起来, 避免 conn 的引用计数为 0 被销毁
        m_connMap[conn->GetID()] = conn;
        conn->_ConnEstablished();
    }
    void _DeleteConnection(const TcpConnPtr& conn) {
        conn->_ConnDestroyed();
        auto n = m_connMap.erase(conn->GetID());
        (void)n;
        assert(n == 1);
    }
    void _InnerMessageCallBack(const TcpConnPtr& conn, Buffer* buf) {
        // 循环是因为 可能缓冲区内已有多个包
        while (buf->GetReadSize() > 0) {
            // Decode 成功 会自动调用 回调
            int res = m_ptrCodec->Decode(conn, buf);
            if (res == 0) {
                LogDebug("res == 0");
                break;
            } else if (res == -1) {
                conn->_ConnDestroyed();
                m_connMap.erase(conn->GetID());
                return;
            }
        }
    }
private:
    Poller* m_ptrPoller;
    std::shared_ptr<Codec<PARAMS...>> m_ptrCodec;
    std::string m_ip;
    uint16_t m_port;
    size_t m_bufferSize;
    std::unique_ptr<TcpAcceptor> m_ptrAcceptor;
    ConnectCallBack m_connectCallBack;
    DisconnectCallBack m_disconnectCallBack;
    // TcpConnPtr 引用, 移除出去时, 有可能会销毁
    std::unordered_map<uint64_t, TcpConnPtr> m_connMap;
};

template <typename... PARAMS>
void TcpServer<PARAMS...>::Send(const TcpConnPtr& conn, uint16_t cmd, const void* data, int dataSize, PARAMS... args) {
    Buffer buffer;
    conn->_GetTcpServer<PARAMS...>()->GetCodec()->Encode(&buffer, cmd, data, dataSize, args...);
    conn->_Send(&buffer);
}

template <typename... PARAMS>
void TcpConn::Send(uint16_t cmd, const void* data, int dataSize, PARAMS... args) {
    TcpServer<PARAMS...>::Send(shared_from_this(), cmd, data, dataSize, args...);
}

}

#endif