#include <iostream>

#include "BearNet/tcp/TcpServer.h"
#include "BearNet/poller/EpollPoller.h"

#include "RawCodec.h"

using namespace std;
using namespace BearNet;


void onConnect(const TcpConnPtr& conn) {
    cout << "onConnect" << endl;
    string sendMsg("aHa 我要飞往天上");
    conn->Send(16, sendMsg.data(), sendMsg.size());
}

void onDisconnect(const TcpConnPtr& conn) {
    cout << "onDisconnect" << endl;
}

void onCmd16(const TcpConnPtr& conn, const std::shared_ptr<std::string>& msg) { 
    cout << msg->data() << endl;
    string sendMsg("aHa 像那天鸟翱翔");
    conn->Send(17, sendMsg.c_str(), sendMsg.size());
}

int main() {
    auto codec = new RawCodec<>();
    std::unique_ptr<Poller> poller(Poller::CreatePoller());

    TcpServer server(poller.get(), codec);

    server.SetConnectCallBack(onConnect);
    server.SetDisconnectCallBack(onDisconnect);

    server.Register<std::string>(16, onCmd16);

    server.Start("0.0.0.0", 1234);
    
    Poller::ChannelList activeChannelList;
    for (;;) {
        activeChannelList.clear();
        poller->Poll(false, true, activeChannelList, -1);
        for (auto channel : activeChannelList) {
            channel->HandleEvent();
        }

        activeChannelList.clear();
        poller->Poll(true, false, activeChannelList, -1);
        for (auto channel : activeChannelList) {
            channel->HandleEvent();
        }
    }
    return 0;
}