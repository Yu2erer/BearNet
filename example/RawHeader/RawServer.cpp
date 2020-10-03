#include <iostream>

#include "BearNet/tcp/TcpServer.h"
#include "BearNet/poller/EpollPoller.h"

#include "RawCodec.h"

using namespace BearNet;


void onConnect(const TcpConnPtr& conn) {
    std::cout << "onConnect" << std::endl;
    std::string sendMsg("aHa 我要飞往天上");
    conn->Send(16, sendMsg.data(), sendMsg.size());
}

void onDisconnect(const TcpConnPtr& conn) {
    std::cout << "onDisconnect" << std::endl;
}

void onCmd16(const TcpConnPtr& conn, const std::shared_ptr<std::string>& msg) { 
    std::cout << msg->data() << std::endl;
    std::string sendMsg("aHa 像那天鸟翱翔");
    conn->Send(17, sendMsg.c_str(), sendMsg.size());
}

int main() {
    std::unique_ptr<RawCodec<int32_t>> codec(new RawCodec<int32_t>());
    std::unique_ptr<Poller> poller(Poller::CreatePoller());

    TcpServer server(poller.get(), codec.get());
    
    // server.SetEncode()...
    // server.SetDecode()...

    server.SetConnectCallBack(onConnect);
    server.SetDisconnectCallBack(onDisconnect);

    server.Register<std::string>(16, onCmd16);


    server.Start("0.0.0.0", 1234);
    
    Poller::ChannelList channelList;

    for (;;) {
        channelList.clear();
        poller->Poll(channelList, -1);
        for (auto channel : channelList) {
            channel->HandleEvent();
        }
    }
    return 0;
}