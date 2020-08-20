#ifndef BEARNET_TCPSERVER_H
#define BEARNET_TCPSERVER_H

#include <vector>

#include "BearNet/TcpConn.h"

namespace BearNet {

class Poller;
class Acceptor;

class TcpServer : private Noncopyable {
public:
    typedef std::function<void (const TcpConn&)> ConnectionCallBack;

    TcpServer(Poller *poller, const std::string& ip, uint16_t port);
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
private:
    void _NewConnection(Socket sock);
private:
    Poller* m_ptrPoller;
    std::string m_ip;
    uint16_t m_port;
    std::unique_ptr<Acceptor> m_ptrAcceptor;
    ConnectionCallBack m_connectionCallBack;
    MessageCallBack m_messageCallBack;
    // 临时 只是为了避免 TcpConnPtr 离开生命期 自动销毁
    // 正常 也应该做一个 TcpConn 的 map
    std::vector<TcpConnPtr> m_vec;
};


}

#endif