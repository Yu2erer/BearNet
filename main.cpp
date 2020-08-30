#include <iostream>
#include <cstring>

#include "BearNet/tcp/TcpServer.h"
#include "BearNet/poller/EpollPoller.h"
#include "BearNet/Channel.h"
#include "BearNet/Buffer.h"
#include "BearNet/codec/Codec.h"

using namespace BearNet;
using namespace std;


void onMessage(const TcpConnPtr conn, Buffer* buf) {
    // string message(buf->GetReadPtr(), buf->GetReadSize());
    // buf->Write(buf->GetReadSize());
    // 学习解包
    // printf("onMessage(): [%s], size: %d\n", message.c_str(), message.size());
    DefaultNetPackHeader header;
    const char* raw = buf->GetReadPtr();
    const size_t size = buf->GetReadSize();
    buf->Write(size);

    memcpy(&header, raw, sizeof(header));
    cout << "Header.tag: " << header.tag << "; Header.size: "  << header.size << "; Header.cmd: " << header.cmd;
    cout << endl;
    string msg(raw + sizeof(header), header.size);
    cout << "Msg: " << msg << endl;
    conn->Send("Hello");
}



int main() {
    std::unique_ptr<Poller> poller(new EpollPoller());

    TcpServer server(poller.get(), "0.0.0.0", 1234);
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