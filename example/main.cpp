#include <iostream>
#include <cstring>

#include "BearNet/tcp/TcpServer.h"
#include "BearNet/poller/EpollPoller.h"
#include "BearNet/Channel.h"
#include "BearNet/Buffer.h"
#include "BearNet/codec/Codec.h"

// #include "codec.h"

using namespace BearNet;
using namespace std;

// // [cmd] 100     req 101
// void onMessage(const TcpConnPtr& conn, Buffer* buf) {
//     // DefaultCodec::instance().Decode(buf->GetReadPtr(), buf->GetReadSize());
    
//     // string message(buf->GetReadPtr(), buf->GetReadSize());
//     // buf->Write(buf->GetReadSize());
//     // // printf("onMessage(): [%s], size: %d\n", message.c_str(), message.size());

//     // char *buffer = nullptr;
//     // uint32_t bufferSize = 0;
//     // DefaultCodec::instance().Encode(16, message.c_str(), message.size(), &buffer, &bufferSize);
//     // conn->GetTcpServer()->GetCmdCallBack(16)(conn);
//     // conn->Send(buffer, 0);
//     // conn->GetTcpServer()->
//     TcpServer::Send(conn, 16, "HelloWorld", 10);
// }

void onConnect(const TcpConnPtr& conn) {
    cout << "onConnect" << endl;
}

void onDisconnect(const TcpConnPtr& conn) {
    cout << "onDisconnect" << endl;
}

void onCmd16(const TcpConnPtr& conn, const std::string& msg) { 
    cout << "cmd 16 succ." << endl;
    cout << msg << endl;
}

int main() {
    std::unique_ptr<Poller> poller(new EpollPoller());

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