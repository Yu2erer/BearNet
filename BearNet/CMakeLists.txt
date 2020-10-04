
set(BearNetSrc
    Channel.cpp
    SocketUtils.cpp
    Buffer.cpp
    poller/Poller.cpp
    poller/EpollPoller.cpp
    tcp/TcpAcceptor.cpp
    tcp/TcpConn.cpp
)

add_library(bearnet ${BearNetSrc})

