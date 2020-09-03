#include <iostream>
#include <cstring>

#include "BearNet/tcp/TcpServer.h"
#include "BearNet/poller/EpollPoller.h"
#include "BearNet/Channel.h"
#include "BearNet/Buffer.h"
#include "BearNet/codec/Codec.h"


using namespace BearNet;
using namespace std;


void onConnect(const TcpConnPtr& conn) {
    cout << "onConnect" << endl;
    conn->Send(16, "test", 4);
}

void onDisconnect(const TcpConnPtr& conn) {
    cout << "onDisconnect" << endl;
}

void onCmd16(const TcpConnPtr& conn, const std::shared_ptr<void>& msg) { 
    cout << "cmd 16 succ." << endl;
    shared_ptr<std::string> p = static_pointer_cast<string>(msg);
    cout << p->data() << endl;
    conn->Send(17);
}

int main() {
    std::unique_ptr<Poller> poller(Poller::CreatePoller());

    TcpServer server(poller.get(), "0.0.0.0", 1234);
    server.SetConnectCallBack(onConnect);
    server.SetDisconnectCallBack(onDisconnect);
    server.Register(16, onCmd16);
    
    server.Start();
    
    Poller::ChannelList activeChannelList;
    for (;;) {
        activeChannelList.clear();
        poller->Poll(false, true, activeChannelList, -1);
        for (auto channel : activeChannelList) {
            channel->HandleEvent();
        }

        // activeChannelList.clear();
        // poller->Poll(true, false, activeChannelList, -1);
        // for (auto channel : activeChannelList) {
            // channel->HandleEvent();
        // }
    }
    return 0;
}