#ifndef BEARNET_ACCEPTOR_H
#define BEARNET_ACCEPTOR_H

#include <functional>


#include "BearNet/base/Noncopyable.h"
#include "BearNet/Channel.h"
#include "BearNet/SocketUtils.h"

namespace BearNet {

class TcpAcceptor : private Noncopyable {
public:
    typedef std::function<void (int fd)> NewConnectionCallBack;

    TcpAcceptor(Poller* poller);
    ~TcpAcceptor();
public:
    bool Listen(const std::string& ip, uint16_t port);

    void SetNewConnectionCallBack(const NewConnectionCallBack& callBack) {
        m_newConnectionCallBack = callBack;
    }
private:
    void _HandleRead();
private:
    int m_acceptFd;
    Channel m_acceptChannel;
    std::string m_ip;
    uint16_t m_port;
    NewConnectionCallBack m_newConnectionCallBack;
};


}

#endif