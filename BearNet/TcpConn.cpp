#include "BearNet/TcpConn.h"
#include "BearNet/Channel.h"

using namespace BearNet;


TcpConn::TcpConn(Poller* poller, Socket sock)
    : m_ptrPoller(poller),
      m_sock(sock),
      m_ptrChannel(new Channel(m_sock, m_ptrPoller)) {
    m_ptrChannel->SetReadCallBack(std::bind(&TcpConn::_HandleRead, this));
}

TcpConn::~TcpConn() {
    m_ptrChannel->Remove();
    SocketHelper::Close(m_sock);
}

void TcpConn::ConnEstablished() {
    m_ptrChannel->EnableReading();
}

void TcpConn::_HandleRead() {
    char buf[65536];
    // -2, > 0, 0, < 0
    int n = SocketHelper::Recv(m_sock, buf, sizeof(buf));
    if (n < 0) {
        if (n == -2) {
            // 正常
        }
        // 错误
    } else if (n == 0) {
        // 关闭连接
        m_ptrChannel->DisableAll();
    } else {
        if (m_messageCallBack) {
            m_messageCallBack(shared_from_this(), buf, n);
        }
    }
}