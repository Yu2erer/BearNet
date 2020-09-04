#include <assert.h>

#include "BearNet/base/Log.h"
#include "BearNet/tcp/TcpServer.h"
#include "BearNet/tcp/TcpAcceptor.h"
#include "BearNet/codec/Codec.h"

using namespace BearNet;


TcpServer::TcpServer(Poller *poller, const std::string& ip, uint16_t port, size_t bufferSize)
    : m_ptrPoller(poller),
      m_ptrCodec(new Codec()),
      m_ip(ip),
      m_port(port),
      m_bufferSize(bufferSize),
      m_ptrAcceptor(new TcpAcceptor(m_ptrPoller, m_ip, m_port)) {
    m_ptrAcceptor->SetNewConnectionCallBack(std::bind(&TcpServer::_NewConnection, this, std::placeholders::_1));
}


TcpServer::~TcpServer() {
    for (auto& item : m_connMap) {
        item.second->_ConnDestroyed();
        item.second.reset();
    }
    m_connMap.clear();
}


void TcpServer::Start() {
    m_ptrAcceptor->Listen();
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
        NetPackage netPackage;
        int res = m_ptrCodec->Decode(conn, buf, &netPackage);
        if (res == 0) {
            LogDebug("res == 0");
            break;
        } else if (res == -1) {
            conn->_ConnDestroyed();
            m_connMap.erase(conn->GetID());
            return;
        }
        // res == 1
        auto callBack = GetCmdCallBack(netPackage.cmd);
        auto p = GetCmdCallBack2(netPackage.cmd);

        // p->OnMessage(conn, )
        if (!callBack) {
            LogDebug("不认识 cmd: %d", netPackage.cmd);
        } else {
            // p->OnMessage(conn, )
            // callBack(conn, netPackage.cmdMsg);
            auto ptr = p->MakePtr();
            ptr->Clear();
            ptr->ParseFromString(netPackage.cmdMsg);
            p->OnMessage(conn, ptr);
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
    TcpServer::Send(conn, cmd, "", 0);
}
