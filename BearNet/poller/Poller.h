#ifndef BEARNET_POLLER_POLLER_H
#define BEARNET_POLLER_POLLER_H

#include <map>
#include <vector>

#include "BearNet/base/Noncopyable.h"

namespace BearNet {

class Channel;

class Poller : private Noncopyable {
public:
    typedef std::vector<Channel*> ChannelList;

    Poller() = default;
    virtual ~Poller() = default;
public:
    virtual bool Poll(ChannelList& activeChannels, int timeoutMs) = 0;

    virtual void UpdateChannel(Channel* channel) = 0;
    virtual void DeleteChannel(Channel* channel) = 0;
    virtual bool HasChannel(Channel* channel) const;

    static Poller* CreatePoller();

protected:
    typedef std::map<int, Channel*> ChannelMap;
    ChannelMap m_channels;
};


}

#endif