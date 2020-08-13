
set(BearNetSrc
    Channel.cpp
    poller/Poller.cpp
    poller/EpollPoller.cpp
)

add_library(BearNet ${BearNetSrc})

