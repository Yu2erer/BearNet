#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <assert.h>
#include <string>
#include <cstdlib>
#include <arpa/inet.h>

#include "BearNet/Socket.h"
#include "BearNet/base/Log.h"

using namespace BearNet;

// Todo: 支持 ipv6
Socket SocketHelper::Create() {
    Socket sock = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sock != InvalidSocket) {
        if (!SetNoDelay(sock)) {
            Close(sock);
            sock = InvalidSocket;
        }

    } else {
        LogErr("SocketHelper::Create() Get Socket fail.");
    }
    return sock;
}

bool SocketHelper::Listen(Socket sock, std::string ip, uint16_t port) {
    assert(sock != InvalidSocket);
    // 地址重用
    if (!SetReuse(sock)) {
        return false;
    }
    
    struct sockaddr addr;
    memset(&addr, 0, sizeof(addr));
    struct sockaddr_in* addr4 = (struct sockaddr_in*)&addr;
    addr4->sin_family = AF_INET;
    addr4->sin_port = htons(port);
    addr4->sin_addr.s_addr = inet_addr(ip.c_str());
    int addrSize = sizeof(struct sockaddr_in);
    
    int ret = ::bind(sock, &addr, addrSize);
    if (ret < 0) {
        LogSysErr("SocketHelper::Listen() Bind Socket fail.");
        return false;
    }
    ret = ::listen(sock, 512);
    if (ret < 0) {
        LogSysErr("SocketHelper::Listen() Listen Socket fail.");
        return false;
    }
    return true;
}

bool SocketHelper::Shutdown(Socket sock, int how) {
    assert(sock != InvalidSocket);

    int ret = shutdown(sock, how);
    if (ret < 0) {
        LogSysErr("SocketHelper::Shutdown() Socket fail.");
        return false;
    }
    LogDebug("Shutdown Socket = %d", sock);

    return true;
}

Socket SocketHelper::Accept(Socket listenSock) {
    assert(listenSock != InvalidSocket);
    struct sockaddr addr;
    socklen_t addrSize = static_cast<socklen_t>(sizeof(addr));

    Socket sock = ::accept4(listenSock, &addr,
                        &addrSize, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (sock == InvalidSocket) {
        LogSysErr("SocketHelper::Accept() Accept fail.");
    }
    return sock;
}

ssize_t SocketHelper::Send(Socket sock, const char *buf, uint32_t size) {
    do {
        ssize_t ret = write(sock, buf, size);
        if (ret >= 0) {
            return ret;
        }
        if (errno == EAGAIN) {
            return 0;
        } else {
            LogSysErr("SocketHelper::Send() Send Socket = %d fail.", sock);
            return -1;
        }
        
    } while (true);
}


ssize_t SocketHelper::Recv(Socket sock, char *buf, uint32_t size) {
    assert(buf != nullptr);

    do {
        ssize_t ret = read(sock, buf, size);
        if (ret >= 0) {
            return ret;
        }
        if (errno == EAGAIN) {
            return -2;
        } else {
            LogSysErr("SocketHelper::Recv() Recv Socket = %d fail.", sock);
            return -1;
        }
        
    } while (true);
}

void SocketHelper::Close(Socket sock) {
    assert(sock != InvalidSocket);
    close(sock);
    LogDebug("Close Socket = %d", sock);
}

bool SocketHelper::SetNoDelay(Socket sock, int flag /* =1 */ ) {
    assert(sock != InvalidSocket);
    int ret = ::setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(flag));
    if (ret < 0) {
        LogErr("SocketHelper::SetNoDelay() Set Socket = %d NoDelay fail.", sock);
        return false;
    }
    return true;
}

bool SocketHelper::SetReuse(Socket sock) {
    assert(sock != InvalidSocket);

    int flag = 1;
    int ret = ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &flag, sizeof(flag));
    if (ret < 0) {
        LogErr("SocketHelper::SetReuse() Set Socket = %d Resuse fail.", sock);
        return false;
    }
    return true;

}
