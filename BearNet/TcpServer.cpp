#include <assert.h>

#include "BearNet/TcpServer.h"
#include "BearNet/base/Log.h"
#include "BearNet/Acceptor.h"

using namespace BearNet;


TcpServer::TcpServer(Poller *poller, const std::string& ip, uint16_t port)
    : m_ptrPoller(poller),
      m_ip(ip),
      m_port(port),
      m_ptrAcceptor(new Acceptor(m_ptrPoller, m_ip, m_port)) {
    m_ptrAcceptor->SetNewConnectionCallBack(std::bind(&TcpServer::_NewConnection, this, std::placeholders::_1));
}


TcpServer::~TcpServer() {
    for (auto& item : m_connMap) {
        item.second->ConnDestroyed();
        item.second.reset();
    }
    m_connMap.clear();
}


void TcpServer::Start() {
    m_ptrAcceptor->Listen();
}


void TcpServer::_NewConnection(Socket sock) {
    LogDebug("TcpServer New Connection");
    TcpConnPtr conn(new TcpConn(m_ptrPoller, sock));
    conn->SetMessageCallBack(m_messageCallBack);
    conn->SetCloseCallBack(std::bind(&TcpServer::_DeleteConnection, this, std::placeholders::_1));
    // 存储起来, 避免 conn 的引用计数为 0 被销毁
    m_connMap[conn->GetID()] = conn;
    conn->ConnEstablished();
}

void TcpServer::_DeleteConnection(const TcpConnPtr& conn) {
    conn->ConnDestroyed();
    auto n = m_connMap.erase(conn->GetID());
    (void)n;
    assert(n == 1);
}