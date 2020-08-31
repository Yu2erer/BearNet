#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <assert.h>
#include <string>
#include <cstdlib>
#include <arpa/inet.h>

#include "BearNet/SocketUtils.h"
#include "BearNet/base/Log.h"

using namespace BearNet;


int SocketUtils::Create() {
    int fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (fd != InvalidSocket) {
        if (!SetNoDelay(fd)) {
            Close(fd);
            fd = InvalidSocket;
        }

    } else {
        LogErr("SocketUtils::Create() Get Socket fail.");
    }
    return fd;
}

bool SocketUtils::Listen(int fd, const std::string& ip, uint16_t port) {
    assert(fd != InvalidSocket);
    // 地址重用
    if (!SetReuse(fd)) {
        return false;
    }
    
    struct sockaddr addr;
    memset(&addr, 0, sizeof(addr));
    struct sockaddr_in* addr4 = (struct sockaddr_in*)&addr;
    addr4->sin_family = AF_INET;
    addr4->sin_port = htons(port);
    addr4->sin_addr.s_addr = inet_addr(ip.c_str());
    int addrSize = sizeof(struct sockaddr_in);
    
    int ret = ::bind(fd, &addr, addrSize);
    if (ret < 0) {
        LogSysErr("SocketUtils::Listen() Bind Socket fail.");
        return false;
    }
    ret = ::listen(fd, 512);
    if (ret < 0) {
        LogSysErr("SocketUtils::Listen() Listen Socket fail.");
        return false;
    }
    return true;
}

bool SocketUtils::Shutdown(int fd) {
    assert(fd != InvalidSocket);

    int ret = ::shutdown(fd, SHUT_WR);
    if (ret < 0) {
        LogSysErr("SocketUtils::Shutdown() Socket fail.");
        return false;
    }
    LogDebug("Shutdown Socket = %d", fd);

    return true;
}

int SocketUtils::Accept(int listenFd) {
    assert(listenFd != InvalidSocket);
    struct sockaddr addr;
    socklen_t addrSize = static_cast<socklen_t>(sizeof(addr));

    int fd = ::accept4(listenFd, &addr,
                        &addrSize, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (fd == InvalidSocket) {
        LogSysErr("SocketUtils::Accept() Accept fail.");
    }
    return fd;
}

void SocketUtils::Close(int fd) {
    assert(fd != InvalidSocket);
    close(fd);
    LogDebug("Close Socket = %d", fd);
}

bool SocketUtils::SetNoDelay(int fd, int flag /* =1 */ ) {
    assert(fd != InvalidSocket);
    int ret = ::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(flag));
    if (ret < 0) {
        LogErr("SocketUtils::SetNoDelay() Set Socket = %d NoDelay fail.", fd);
        return false;
    }
    return true;
}

bool SocketUtils::SetReuse(int fd) {
    assert(fd != InvalidSocket);

    int flag = 1;
    int ret = ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &flag, sizeof(flag));
    if (ret < 0) {
        LogErr("SocketUtils::SetReuse() Set Socket = %d Resuse fail.", fd);
        return false;
    }
    return true;

}

bool SocketUtils::SetKeepAlive(int fd, uint32_t keepAliveTime) {
    assert(fd != InvalidSocket);

    // Todo: 返回值判断
    // open keep alive
    int optval = 1;
    ::setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
    // 当多久没有数据时 发送探测
    int keepIdle = static_cast<int>(keepAliveTime);
    ::setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle));
    // 重试间隔
    int keepIntvl = 1;
    ::setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, (void*)&keepIntvl, sizeof(keepIntvl));
    // 重试次数
    int keepCnt = 1;
    ::setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, (void*)&keepCnt, sizeof(keepCnt));
    
    return true;
}

