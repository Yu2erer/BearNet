#ifndef BEARNET_CHANNEL_H
#define BEARNET_CHANNEL_H

#include <sys/poll.h>
#include <functional>
#include <memory>

#include "BearNet/base/Noncopyable.h"

namespace BearNet {

class Poller;

class Channel : private Noncopyable {
public:
    typedef std::function<void()> EventCallBack;
    enum InPollerStatus {
        // 在 poller 中未添加
        IN_POLLER_NEW = -1,
        // 在 poller 中已添加
        IN_POLLER_ADDED = 1,
        // 在 poller 中已删除
        IN_POLLER_DELETED = 2,
    };
public:
    Channel(int fd, Poller* poller);
    ~Channel();
public:
    int Getfd() const { return m_fd; }
    uint32_t GetEvents() const { return m_events; }
    void Remove();
public:
    void HandleEvent();

    bool IsNoneEvent() const { return m_events == 0; }
    void DisableAll() { m_events = 0; _Update(); }

    // 不需要 POLLERR 和 POLLHUB, 内核会自动为我们加上
    void EnableReading() { m_events |= POLLIN; _Update(); }
    void DisableReading() { m_events &= ~POLLIN; _Update(); }
    bool IsReading() const { return m_events & POLLIN; }

    void EnableWriting() { m_events |= POLLOUT; _Update(); }
    void DisableWriting() { m_events &= ~POLLOUT; _Update(); }
    bool IsWriting() const { return m_events & POLLOUT; }
public:

    // used by poller
    void SetRevents(int revt) { m_revents = revt; }
    // channel 在 poller 中的状态
    InPollerStatus GetInPollerStatus() { return m_inPollerStatus; }
    void SetInPollerStatus(InPollerStatus inPollerStatus) { m_inPollerStatus = inPollerStatus; }

private:
    void _Update();
public:
    void SetReadCallBack(EventCallBack callBack) {
        m_readCallBack = std::move(callBack);
    }
    void SetWriteCallBack(EventCallBack callBack) {
        m_writeCallBack = std::move(callBack);
    }
    void SetCloseCallBack(EventCallBack callBack) {
        m_closeCallBack = std::move(callBack);
    }
    void SetErrorCallBack(EventCallBack callBack) {
        m_errorCallBack = std::move(callBack);
    }


private:
    const int m_fd;
    std::unique_ptr<Poller> m_ptrPoller;
    uint32_t m_events;
    uint32_t m_revents;
    InPollerStatus m_inPollerStatus;

    EventCallBack m_readCallBack;
    EventCallBack m_writeCallBack;
    EventCallBack m_closeCallBack;
    EventCallBack m_errorCallBack;
};

}

#endif
