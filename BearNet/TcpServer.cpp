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

}


void TcpServer::Start() {
    m_ptrAcceptor->Listen();
}


void TcpServer::_NewConnection(Socket sock) {
    LogDebug("TcpServer New Connection");
    TcpConnPtr conn(new TcpConn(m_ptrPoller, sock));
    conn->SetMessageCallBack(m_messageCallBack);
    conn->ConnEstablished();
    m_vec.push_back(conn);
}
