#include "BearNet/poller/Poller.h"
#include "BearNet/poller/EpollPoller.h"
#include "BearNet/Channel.h"

using namespace BearNet;

Poller* Poller::CreatePoller() {
    return new EpollPoller();
}

bool Poller::HasChannel(Channel* channel) const {
  ChannelMap::const_iterator it = m_channels.find(channel->Getfd());
  return it != m_channels.end() && it->second == channel;
}
