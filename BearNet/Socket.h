#ifndef BEARNET_SOCKET_H
#define BEARNET_SOCKET_H

#include <cstdint>
#include <sys/types.h>

namespace BearNet {

#ifndef InvalidSocket
#define InvalidSocket -1
#endif

class SocketUtils {
public:
    // 默认设置 NoDelay 和 NoBlock
    static int Create();
    static bool Listen(int fd, const std::string& ip, uint16_t port);
    static int Accept(int listenFd);
    
    static bool Shutdown(int fd, int how);
    static void Close(int fd);

    static bool SetNoDelay(int fd, int flag = 1);
    static bool SetReuse(int fd);
    static bool SetKeepAlive(int fd, uint32_t keepAliveTime = 60);
public:
    SocketUtils() = delete;
    ~SocketUtils() = delete;
};

}
#endif