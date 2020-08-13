#ifndef BEARNET_SOCKET_H
#define BEARNET_SOCKET_H

#include <cstdint>
#include <sys/types.h>

#include "BearNet/base/Noncopyable.h"

namespace BearNet {

typedef int Socket;

#ifndef InvalidSocket
#define InvalidSocket -1
#endif

class SocketHelper {
public:
    // 默认设置 NoDelay 和 NoBlock
    static Socket Create();
    static bool Listen(Socket sock, std::string ip, uint16_t port);
    static Socket Accept(Socket listenSock);
    static ssize_t Send(Socket sock, const char *buf, uint32_t size);
    static ssize_t Recv(Socket sock, char *buf, uint32_t size);
    
    static bool Shutdown(Socket sock, int how);
    static void Close(Socket sock);

    static bool SetNoDelay(Socket sock, int flag = 1);
    static bool SetReuse(Socket sock);
    static bool SetKeepAlive(Socket sock, uint32_t keepAliveTime = 60);
public:
    SocketHelper() = delete;
    ~SocketHelper() = delete;
};

}
#endif