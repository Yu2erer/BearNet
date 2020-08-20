#include <iostream>

#include "BearNet/TcpServer.h"
#include "BearNet/poller/EpollPoller.h"
#include "BearNet/Channel.h"

using namespace BearNet;
using namespace std;


void onMessage(const TcpConnPtr conn, char* buf, int n) {
    cout << "On Message: received " << n << " bytes, data: " << buf << endl;
}



int main() {
    std::unique_ptr<Poller> poller(new EpollPoller());

    TcpServer server(poller.get(), "0.0.0.0", 1234);
    server.SetMessageCallback(onMessage);

    server.Start();
    
    Poller::ChannelList activeChannelList;
    for (;;) {
        activeChannelList.clear();
        poller->Poll(false, true, activeChannelList, 10000);
        for (auto channel : activeChannelList) {
            channel->HandleEvent();
        }

        activeChannelList.clear();
        poller->Poll(true, false, activeChannelList, 10000);
        for (auto channel : activeChannelList) {
            channel->HandleEvent();
        }
    }
    return 0;
}