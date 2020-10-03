
set(BearNetSrc
    Channel.cpp
    SocketUtils.cpp
    Buffer.cpp
    NetHandle.cpp
    poller/Poller.cpp
    poller/EpollPoller.cpp
    tcp/TcpAcceptor.cpp
    tcp/TcpConn.cpp
    tcp/TcpServer.cpp
)

add_library(bearnet ${BearNetSrc})

