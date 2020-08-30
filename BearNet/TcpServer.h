#ifndef BEARNET_TCPSERVER_H
#define BEARNET_TCPSERVER_H

#include <unordered_map>

#include "BearNet/TcpConn.h"

namespace BearNet {

class Poller;
class Acceptor;

class TcpServer : private Noncopyable {
public:
    TcpServer(Poller *poller, const std::string& ip, uint16_t port, size_t bufferSize = 10);
    ~TcpServer();
public:
    void Start();
    void Stop();
public:
    void SetConnectionCallBack(const ConnectionCallBack& callBack) {
        m_connectionCallBack = callBack;
    }
    void SetMessageCallback(const MessageCallBack& callBack) {
        m_messageCallBack = callBack;
    }
    void SetMessageCodec();
private:
    void _NewConnection(int fd);
    void _DeleteConnection(const TcpConnPtr&);
private:
    Poller* m_ptrPoller;
    std::string m_ip;
    uint16_t m_port;
    size_t m_bufferSize;
    std::unique_ptr<Acceptor> m_ptrAcceptor;
    ConnectionCallBack m_connectionCallBack;
    MessageCallBack m_messageCallBack;
    // TcpConnPtr 引用, 移除出去时, 有可能会销毁
    std::unordered_map<uint64_t, TcpConnPtr> m_connMap;
};


}

#endif