
set(BearNetSrc
    Channel.cpp
    poller/Poller.cpp
    poller/EpollPoller.cpp
    Socket.cpp
    Acceptor.cpp
    TcpConn.cpp
    TcpServer.cpp
    Buffer.cpp
)

add_library(BearNet ${BearNetSrc})

