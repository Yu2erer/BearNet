#ifndef BEARNET_TCPSERVER_H
#define BEARNET_TCPSERVER_H

#include <unordered_map>

#include "BearNet/tcp/TcpConn.h"

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
    void SetConnectCallBack(const ConnectCallBack& callBack) {
        m_connectCallBack = callBack;
    }
    void SetDisconnectCallBack(const DisconnectCallBack& callBack) {
        m_disconnectCallBack = callBack;
    }
    void SetMessageCallback(const MessageCallBack& callBack) {
        m_messageCallBack = callBack;
    }
private:
    void _NewConnection(int fd);
    void _DeleteConnection(const TcpConnPtr&);
private:
    Poller* m_ptrPoller;
    std::string m_ip;
    uint16_t m_port;
    size_t m_bufferSize;
    std::unique_ptr<Acceptor> m_ptrAcceptor;
    ConnectCallBack m_connectCallBack;
    DisconnectCallBack m_disconnectCallBack;
    MessageCallBack m_messageCallBack;
    // TcpConnPtr 引用, 移除出去时, 有可能会销毁
    std::unordered_map<uint64_t, TcpConnPtr> m_connMap;
};


}

#endif