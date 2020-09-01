
set(BearNetSrc
    Channel.cpp
    SocketUtils.cpp
    Buffer.cpp
    NetHandle.cpp
    codec/Codec.cpp
    poller/Poller.cpp
    poller/EpollPoller.cpp
    tcp/Acceptor.cpp
    tcp/TcpConn.cpp
    tcp/TcpServer.cpp
)

add_library(BearNet ${BearNetSrc})

