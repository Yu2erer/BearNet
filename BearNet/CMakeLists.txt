
set(BearNetSrc
    Channel.cpp
    poller/Poller.cpp
    poller/EpollPoller.cpp
    Socket.cpp
    Acceptor.cpp
)

add_library(BearNet ${BearNetSrc})

