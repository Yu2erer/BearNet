#include <iostream>
#include <cstring>

#include "BearNet/tcp/TcpServer.h"
#include "BearNet/poller/EpollPoller.h"
#include "BearNet/Channel.h"
#include "BearNet/Buffer.h"

#include "codec.h"

using namespace BearNet;
using namespace std;


void onMessage(const TcpConnPtr& conn, Buffer* buf) {
    DefaultCodec::instance().Decode(buf->GetReadPtr(), buf->GetReadSize());
    
    string message(buf->GetReadPtr(), buf->GetReadSize());
    buf->Write(buf->GetReadSize());
    // printf("onMessage(): [%s], size: %d\n", message.c_str(), message.size());

    char *buffer = nullptr;
    uint32_t bufferSize = 0;
    DefaultCodec::instance().Encode(16, message.c_str(), message.size(), &buffer, &bufferSize);

    conn->GetTcpServer()->Send(13);
    conn->Send(buffer, bufferSize);
}

void onConnect(const TcpConnPtr& conn) {
    cout << "onConnect" << endl;
}

void onDisconnect(const TcpConnPtr& conn) {
    cout << "onDisconnect" << endl;
}

int main() {
    std::unique_ptr<Poller> poller(new EpollPoller());

    TcpServer server(poller.get(), "0.0.0.0", 1234);
    server.SetConnectCallBack(onConnect);
    server.SetDisconnectCallBack(onDisconnect);
    server.SetMessageCallback(onMessage);
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