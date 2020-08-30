#ifndef BEARNET_ACCEPTOR_H
#define BEARNET_ACCEPTOR_H

#include <functional>


#include "BearNet/base/Noncopyable.h"
#include "BearNet/Channel.h"
#include "BearNet/Socket.h"

namespace BearNet {

class Acceptor : private Noncopyable {
public:
    typedef std::function<void (int fd)> NewConnectionCallBack;

    Acceptor(Poller* poller, const std::string& ip, uint16_t port);
    ~Acceptor();
public:
    bool Listen();

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