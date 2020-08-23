#include "BearNet/TcpConn.h"
#include "BearNet/Channel.h"

#include "BearNet/base/Log.h"

using namespace BearNet;


TcpConn::TcpConn(Poller* poller, Socket sock)
    : m_ptrPoller(poller),
      m_sock(sock),
      m_ptrChannel(new Channel(m_sock, m_ptrPoller)),
      // Todo: 缓冲区大小应该在 tcpServer 中可配置
      m_recvBuf(10),
      m_sendBuf(10) {

    static uint64_t id = 0;
    m_id = ++ id;

    m_ptrChannel->SetReadCallBack(std::bind(&TcpConn::_HandleRead, this));
    m_ptrChannel->SetWriteCallBack(std::bind(&TcpConn::_HandleWrite, this));
    m_ptrChannel->SetCloseCallBack(std::bind(&TcpConn::_HandleClose, this));
    m_ptrChannel->SetErrorCallBack(std::bind(&TcpConn::_HandleError, this));
    
    LogTrace("TcpConn::TcpConn");
}

TcpConn::~TcpConn() {
    LogTrace("TcpConn::~TcpConn");
    m_ptrChannel->Remove();
    SocketHelper::Close(m_sock);
}

void TcpConn::ConnEstablished() {
    m_ptrChannel->EnableReading();
}

void TcpConn::ConnDestroyed() {
    LogTrace("TcpConn::ConnDestroyed()");
    // Todo: 根据conn状态决定 disableAll()
    // m_ptrChannel->DisableAll();
    m_ptrChannel->Remove();
}

void TcpConn::_HandleRead() {
    ssize_t n = m_recvBuf.ReadFd(m_ptrChannel->Getfd());
    if (n > 0) {
        if (m_messageCallBack) {
            m_messageCallBack(shared_from_this(), &m_recvBuf);
        }
    } else if (n == 0) {
        _HandleClose();
    } else {
        _HandleError();
    }

    // char buf[65536];
    // -2, > 0, 0, < 0
    // int n = SocketHelper::Recv(m_sock, buf, sizeof(buf));

    // if (n < 0) {
    //     if (n == -2) {
    //         // 正常
    //         return;
    //     }
    //     // 错误
    //     _HandleError();
    // } else if (n == 0) {
    //     // 关闭连接
    //     _HandleClose();
    // } else {
    //     // 最好在这里将数据 Decode 后 再还给 TcpServer Or TcpClient
    //     if (m_messageCallBack) {
    //         m_messageCallBack(shared_from_this(), buf);
    //     }
    // }
}

void TcpConn::_HandleWrite() {

}

void TcpConn::_HandleClose() {
    LogTrace("TcpConn::_HandleClose()");
    m_ptrChannel->DisableAll();
    if (m_closeCallBack) {
        m_closeCallBack(shared_from_this());
    }
}

void TcpConn::_HandleError() {
    LogErr("TcpConn::_HandleError");
}
