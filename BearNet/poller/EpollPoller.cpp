#include <sys/epoll.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>

#include "BearNet/poller/EpollPoller.h"
#include "BearNet/Channel.h"

#include "BearNet/base/Log.h"


using namespace BearNet;

// Todo: 将 magic number:16 修改为可配置项
EpollPoller::EpollPoller() 
    : m_readEpollfd(::epoll_create1(EPOLL_CLOEXEC)),
      m_writeEpollfd(::epoll_create1(EPOLL_CLOEXEC)),
      m_readEvents(16),
      m_writeEvents(16) {
    if (m_readEpollfd < 0 || m_writeEpollfd < 0) {
        LogFatal("EpollPoller::EpollPoller()");
    }
}

EpollPoller::~EpollPoller() {
    ::close(m_readEpollfd);
    ::close(m_writeEpollfd);
}

bool EpollPoller::Poll(bool pollWrite, bool pollRead, ChannelList& activeChannels, int timeoutMs) {
    if (pollWrite) {
        _Poll(POLLER_TYPE_WRITE, activeChannels, timeoutMs);
    }

    if (pollRead) {
        _Poll(POLLER_TYPE_READ, activeChannels, timeoutMs);
    }

    return true;
}

bool EpollPoller::_Poll(PollerType pollerType, ChannelList& activeChannels, int timeoutMs) {
    assert(pollerType == POLLER_TYPE_READ || pollerType == POLLER_TYPE_WRITE);

    bool result = false;

    int epollfd = -1;
    EventList& events = m_readEvents;

    if (pollerType == POLLER_TYPE_READ) {
        epollfd = m_readEpollfd;
        events = m_readEvents;
    } else if (pollerType == POLLER_TYPE_WRITE) {
        epollfd = m_writeEpollfd;
        events = m_writeEvents;
    }

    int pollCounts = ::epoll_wait(epollfd, events.data(), static_cast<int>(events.size()), timeoutMs);
    int saveErrno = errno;

    if (pollCounts == -1) {
        if (saveErrno != EINTR) {
            errno = saveErrno;
            LogSysErr("EpollPoller::_poll()");
            goto Exit0;
        }
    } else if (pollCounts == 0) {
        LogTrace("EpollPoller::_Poll() Nothing happend");
        goto Exit1;
    }

    for (int i = 0; i < pollCounts; i ++) {
        // 将 事件传给 channel 让 channel 做事件分发
        Channel* channel = static_cast<Channel*>(events[i].data.ptr);
        channel->SetRevents(events[i].events);
        activeChannels.push_back(channel);
    }

    if (static_cast<size_t>(pollCounts) == events.size()) {
        events.resize(events.size() * 2);
    }

Exit1:
    result = true;
Exit0:
    return result;
}

void EpollPoller::UpdateChannel(Channel* channel) {
    auto status = channel->GetInPollerStatus();
    int fd = channel->Getfd();

    // 新创建 或者 被删除
    if (status == Channel::InPollerStatus::IN_POLLER_NEW 
            || status == Channel::InPollerStatus::IN_POLLER_DELETED) {

        if (status == Channel::InPollerStatus::IN_POLLER_NEW) {
            assert(m_channels.find(fd) == m_channels.end());
            m_channels[fd] = channel;
        } else {
            assert(m_channels.find(fd) != m_channels.end());
            assert(m_channels[fd] == channel);
        }
        LogTrace("New Channel fd = %d", fd);
        channel->SetInPollerStatus(Channel::InPollerStatus::IN_POLLER_ADDED);
        _UpdateEvent(EPOLL_CTL_ADD, channel);

    } else {

        assert(m_channels.find(fd) != m_channels.end());
        assert(m_channels[fd] == channel);
        assert(status == Channel::InPollerStatus::IN_POLLER_ADDED);
        // 没有事件就删除
        if (channel->IsNoneEvent()) {
            _UpdateEvent(EPOLL_CTL_DEL, channel);
            LogTrace("Delete Channel fd = %d", fd);
            channel->SetInPollerStatus(Channel::InPollerStatus::IN_POLLER_DELETED);
        } else {
            LogTrace("Update Channel fd = %d", fd);
            _UpdateEvent(EPOLL_CTL_MOD, channel);
        }
    }
}

void EpollPoller::DeleteChannel(Channel* channel) {
    int fd = channel->Getfd();
    LogTrace("fd = %d", fd);
    
    Channel::InPollerStatus status = channel->GetInPollerStatus();
    size_t n = m_channels.erase(fd);
    (void)n;
    // 只有在中间状态 才会真正执行删除
    if (status == Channel::InPollerStatus::IN_POLLER_ADDED) {
        _UpdateEvent(EPOLL_CTL_DEL, channel);
    }
    channel->SetInPollerStatus(Channel::InPollerStatus::IN_POLLER_NEW);
}


void EpollPoller::_UpdateEvent(int operation, Channel* channel) {
    assert(operation == EPOLL_CTL_ADD || operation == EPOLL_CTL_MOD || operation == EPOLL_CTL_DEL);

    if (!_CtlEpoll(m_readEpollfd, operation, channel)) {
        printf("EpollPoller::_UpdateEvent() op = %s readEpoll fail. fd: %d\n", _OperationToString(operation), channel->Getfd());
    }
    if (!_CtlEpoll(m_writeEpollfd, operation, channel)) {
        printf("EpollPoller::_UpdateEvent() op = %s writeEpoll fail. fd: %d\n", _OperationToString(operation), channel->Getfd());
    }

}

bool EpollPoller::_CtlEpoll(int epollfd, int operation, Channel* channel) {
    struct epoll_event event = {channel->GetEvents(), {0}};
    event.data.ptr = channel;

    int fd = channel->Getfd();
    int ret = ::epoll_ctl(epollfd, operation, fd, &event);

    return ret == 0;
}


const char* EpollPoller::_OperationToString(int operation) {
  switch (operation) {
    case EPOLL_CTL_ADD:
      return "ADD";
    case EPOLL_CTL_DEL:
      return "DEL";
    case EPOLL_CTL_MOD:
      return "MOD";
    default:
      assert(false && "ERROR op");
      return "Unknown Operation";
  }
}
