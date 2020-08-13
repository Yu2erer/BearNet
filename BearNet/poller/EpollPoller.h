#ifndef BEARNET_POLLER_EPOLLPOLLER_H
#define BEARNET_POLLER_EPOLLPOLLER_H

#include <vector>

#include "BearNet/poller/Poller.h"

struct epoll_event;

namespace BearNet {


class EpollPoller : public Poller {
public:
    EpollPoller();
    ~EpollPoller() override;
public:
    bool Poll(bool pollWrite, bool pollRead,  ChannelList& activeChannels, int timeoutMs) override;

    void UpdateChannel(Channel* channel) override;
    void DeleteChannel(Channel* channel) override;

private:
    bool _Poll(PollerType pollerType, ChannelList& activeChannels, int timeoutMs);
    

    void _UpdateEvent(int operation, Channel* channel);
    bool _CtlEpoll(int epollfd, int operation, Channel* channel);
private:
    const char* _OperationToString(int operation);
private:
    typedef std::vector<struct epoll_event> EventList;

    int m_readEpollfd;
    int m_writeEpollfd;
    EventList m_readEvents;
    EventList m_writeEvents;
};



}

#endif