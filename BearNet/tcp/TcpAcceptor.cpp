#include "BearNet/tcp/TcpAcceptor.h"
#include "BearNet/base/Log.h"

using namespace BearNet;

TcpAcceptor::TcpAcceptor(Poller* poller, const std::string& ip, uint16_t port) 
    : m_acceptFd(SocketUtils::Create()), 
      m_acceptChannel(m_acceptFd, poller),
      m_ip(ip),
      m_port(port) {
    m_acceptChannel.SetReadCallBack(std::bind(&TcpAcceptor::_HandleRead, this));

}

TcpAcceptor::~TcpAcceptor() {
  m_acceptChannel.DisableAll();
  m_acceptChannel.Remove();
}

bool TcpAcceptor::Listen() {
    bool ret = SocketUtils::Listen(m_acceptFd, m_ip, m_port);
    if (!ret) {
        return false;
    }
    m_acceptChannel.EnableReading();
    return true;
}

void TcpAcceptor::_HandleRead() {
    int fd = SocketUtils::Accept(m_acceptFd);
    if (fd != InvalidSocket) {
        if (m_newConnectionCallBack) {
            m_newConnectionCallBack(fd);
        } else {
            SocketUtils::Close(fd);
        }
    } else {
        LogErr("TcpAcceptor::_HandleRead() Accept fail.");
    }
}

