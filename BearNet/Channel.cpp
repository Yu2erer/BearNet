#include <poll.h>
#include <assert.h>

#include "BearNet/Channel.h"
#include "BearNet/poller/Poller.h"
#include "BearNet/base/Log.h"

using namespace BearNet;


Channel::Channel(int fd, Poller* poller)
    : m_fd(fd),
      m_ptrPoller(poller), 
      m_events(0), 
      m_revents(0), 
      m_inPollerStatus(IN_POLLER_NEW) {

}

Channel::~Channel() {
    assert(!m_ptrPoller->HasChannel(this));
}

void Channel::Remove() {
    m_ptrPoller->DeleteChannel(this);
}

void Channel::_Update() {
    m_ptrPoller->UpdateChannel(this);
}

void Channel::HandleEvent() {
    if (m_revents & POLLHUP && !(m_revents & POLLIN)) {
        LogTrace("POLLHUP");
        if (m_closeCallBack) {
            m_closeCallBack();
        }
    }

    if (m_revents & (POLLERR | POLLNVAL)) {
        LogTrace("POLLERR | POLLNVAL");
        if (m_errorCallBack) {
            m_errorCallBack();
        }
    }

    if (m_revents & POLLIN) {
        LogTrace("POLLIN");
        if (m_readCallBack) {
            m_readCallBack();
        }
    }
    
    if (m_revents & POLLOUT) {
        LogTrace("POLLOUT");
        if (m_writeCallBack) {
            m_writeCallBack();
        }
    }
}