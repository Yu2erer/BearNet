#include <iostream>

#include "BearNet/tcp/TcpServer.h"
#include "BearNet/poller/EpollPoller.h"

using namespace BearNet;


void onConnect(const TcpConnPtr& conn) {
    std::cout << "onConnect" << std::endl;
    std::string sendMsg("aHa 我要飞往天上");
    int b = 3;
    conn->Send(16, sendMsg.data(), sendMsg.size(), b);
}

void onDisconnect(const TcpConnPtr& conn) {
    std::cout << "onDisconnect" << std::endl;
}

void onCmd16(const TcpConnPtr& conn, const std::shared_ptr<std::string>& msg) { 
    std::cout << msg->data() << std::endl;
    std::string sendMsg("aHa 像那天鸟翱翔");
    // int b = 3;
    conn->Send(17, sendMsg.c_str(), sendMsg.size());
    // LogDebug("%d", a);
}

int Encode(Buffer* buffer, const void* data, int dataSize) {
    // buffer->AppendToNet(a);
    buffer->Append(data, dataSize);
    return dataSize;
}

int Decode(const TcpConnPtr& conn, Buffer* buffer, int dataSize, const std::shared_ptr<CmdCallBack<>>& callBack) {
    auto ptr = callBack->MakePtr();
    auto message = std::static_pointer_cast<std::string>(ptr);
    // int a = buffer->ReadInt32();
    auto msg = buffer->ReadString(dataSize);
    message->assign(msg);
    callBack->OnMessage(conn, message);
    return 1;
}

int main() {
    std::unique_ptr<Poller> poller(Poller::CreatePoller());
    
    TcpServer<> server(poller.get());
    
    server.SetEncodeFunc(Encode);
    server.SetDecodeFunc(Decode);

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