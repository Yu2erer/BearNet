#include <assert.h>

#include "BearNet/tcp/TcpConn.h"
#include "BearNet/Channel.h"
#include "BearNet/base/Log.h"
#include "BearNet/tcp/TcpServer.h"

using namespace BearNet;


TcpConn::TcpConn(TcpServer* tcpServer, const int fd, size_t bufferSize)
    : m_tcpServer(tcpServer),
      m_ptrPoller(tcpServer->GetPoller()),
      m_state(kConnecting),
      m_fd(fd),
      m_ptrChannel(new Channel(m_fd, m_ptrPoller)),
      m_recvBuf(bufferSize),
      m_sendBuf(bufferSize) {

    static uint64_t id = 0;
    m_id = ++ id;

    SocketUtils::SetKeepAlive(m_fd);

    m_ptrChannel->SetReadCallBack(std::bind(&TcpConn::_HandleRead, this));
    m_ptrChannel->SetWriteCallBack(std::bind(&TcpConn::_HandleWrite, this));
    m_ptrChannel->SetCloseCallBack(std::bind(&TcpConn::_HandleClose, this));
    m_ptrChannel->SetErrorCallBack(std::bind(&TcpConn::_HandleError, this));
    
    LogTrace("TcpConn::TcpConn");
}

TcpConn::~TcpConn() {
    assert(m_state == kDisconnected);
    LogTrace("TcpConn::~TcpConn");
    m_ptrChannel->Remove();
    SocketUtils::Close(m_fd);
}

void TcpConn::_ConnEstablished() {
    assert(m_state == kConnecting);
    m_state = kConnected;
    m_ptrChannel->EnableReading();
    if (m_connectCallBack) {
        m_connectCallBack(shared_from_this());
    }
}

void TcpConn::_ConnDestroyed() {
    LogTrace("TcpConn::ConnDestroyed()");
    if (m_state == kConnected) {
        m_state = kDisconnected;
        m_ptrChannel->DisableAll();
    }
    m_ptrChannel->Remove();
    if (m_disconnectCallBack) {
        m_disconnectCallBack(shared_from_this());
    }
}

void TcpConn::ShutDown() {
    if (m_state != kConnected) {
        return;
    }

    m_state = kDisconecting;
    if (!m_ptrChannel->IsWriting()) {
        SocketUtils::Shutdown(m_fd);
    }
}

void TcpConn::Send(uint16_t cmd) {
    TcpServer::Send(shared_from_this(), cmd);
}

void TcpConn::Send(uint16_t cmd, const void* data, int32_t dataSize) {
    TcpServer::Send(shared_from_this(), cmd, data, dataSize);
}

const std::shared_ptr<CmdCallBack> TcpConn::GetCmdCallBack(uint16_t cmd) const {
    return _GetTcpServer()->GetCmdCallBack(cmd);
}

void TcpConn::_Send(const void* data, uint32_t size) {
    _Send(std::string(static_cast<const char*>(data), size));
}

void TcpConn::_Send(const std::string& message) {
    if (m_state != kConnected || message.empty()) {
        return;
    }

    m_sendBuf.Append(message.data(), message.size());
    if (!m_ptrChannel->IsWriting()) {
        m_ptrChannel->EnableWriting();
    }
}

void TcpConn::_Send(Buffer* buffer) {
    _Send(buffer->GetReadPtr(), buffer->GetReadSize());
    buffer->WriteAll();
}

void TcpConn::_HandleRead() {
    ssize_t n = m_recvBuf.ReadFd(m_fd);
    if (n > 0) {
        m_innerMessageCallBack(shared_from_this(), &m_recvBuf);
    } else if (n == 0) {
        _HandleClose();
    } else {
        LogErr("Err: %d", n);
        _HandleError();
    }
}

void TcpConn::_HandleWrite() {
    if (!m_ptrChannel->IsWriting()) {
        LogTrace("TcpConn is down");
        return;
    }

    ssize_t n = m_sendBuf.WriteFd(m_fd);

    if (n > 0) {
        if (m_sendBuf.GetReadSize() == 0) {
            m_ptrChannel->DisableWriting();
            // 写完了 如果已经被设置为shutdown的话, 要进行shutdown
            // 不要调用类中的 shutdown 接口 会出问题
            if (m_state == kDisconecting) {
                SocketUtils::Shutdown(m_fd);
            }
        }
    } else {
        LogErr("TcpConn::_HandleWrite()");
    }
}

void TcpConn::_HandleClose() {
    assert(m_state == kConnected || m_state == kDisconecting);
    LogTrace("TcpConn::_HandleClose()");
    m_state = kDisconnected;
    m_ptrChannel->DisableAll();
    m_innerCloseCallBack(shared_from_this());
}

void TcpConn::_HandleError() {
    LogErr("TcpConn::_HandleError");
}
