#include "BearNet/Acceptor.h"
#include "BearNet/base/Log.h"

using namespace BearNet;

Acceptor::Acceptor(Poller* poller, std::string ip, uint16_t port) 
    : m_acceptSocket(SocketHelper::Create()), 
      m_acceptChannel(m_acceptSocket, poller),
      m_ip(ip),
      m_port(port) {
    m_acceptChannel.SetReadCallBack(std::bind(&Acceptor::_HandleRead, this));

}

Acceptor::~Acceptor() {
  m_acceptChannel.DisableAll();
  m_acceptChannel.Remove();
}

bool Acceptor::Listen() {
    bool ret = SocketHelper::Listen(m_acceptSocket, m_ip, m_port);
    if (!ret) {
        return false;
    }
    m_acceptChannel.EnableReading();
    return true;
}

void Acceptor::_HandleRead() {
    Socket sock = SocketHelper::Accept(m_acceptSocket);
    if (sock != InvalidSocket) {
        if (m_newConnectionCallBack) {
            m_newConnectionCallBack(sock);
        } else {
            SocketHelper::Close(sock);
        }
    } else {
        LogErr("Acceptor::_HandleRead() Accept fail.");
    }
}

