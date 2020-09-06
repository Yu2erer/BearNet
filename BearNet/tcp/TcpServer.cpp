#include <assert.h>

#include "BearNet/base/Log.h"
#include "BearNet/tcp/TcpServer.h"
#include "BearNet/tcp/TcpAcceptor.h"

using namespace BearNet;


TcpServer::TcpServer(Poller* poller, Codec* codec, size_t bufferSize)
    : m_ptrPoller(poller),
      m_ptrCodec(codec),
      m_ip(),
      m_port(0),
      m_bufferSize(bufferSize),
      m_ptrAcceptor(new TcpAcceptor(m_ptrPoller)) {
    m_ptrAcceptor->SetNewConnectionCallBack(std::bind(&TcpServer::_NewConnection, this, std::placeholders::_1));
}


TcpServer::~TcpServer() {
    for (auto& item : m_connMap) {
        item.second->_ConnDestroyed();
        item.second.reset();
    }
    m_connMap.clear();
}


void TcpServer::Start(const std::string& ip, uint16_t port) {
    m_ip = ip;
    m_port = port;
    m_ptrAcceptor->Listen(m_ip, m_port);
}

void TcpServer::_NewConnection(int fd) {
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

void TcpServer::_InnerMessageCallBack(const TcpConnPtr& conn, Buffer* buf) {
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

void TcpServer::_DeleteConnection(const TcpConnPtr& conn) {
    conn->_ConnDestroyed();
    auto n = m_connMap.erase(conn->GetID());
    (void)n;
    assert(n == 1);
}

void TcpServer::Send(const TcpConnPtr& conn, uint16_t cmd, const void* data, int32_t dataSize) {
    Buffer buffer;
    conn->_GetTcpServer()->GetCodec()->Encode(&buffer, cmd, data, dataSize);
    conn->_Send(&buffer);
}

void TcpServer::Send(const TcpConnPtr& conn, uint16_t cmd) {
    TcpServer::Send(conn, cmd, nullptr, 0);
}
