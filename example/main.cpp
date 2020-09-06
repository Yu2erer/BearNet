#include <iostream>
#include <cstring>

#include "BearNet/tcp/TcpServer.h"
#include "BearNet/poller/EpollPoller.h"
#include "BearNet/Channel.h"
#include "BearNet/Buffer.h"
#include "BearNet/codec/Codec.h"

#include "example/example.pb.h"


using namespace BearNet;
using namespace std;


void onConnect(const TcpConnPtr& conn) {
    cout << "onConnect" << endl;
    // BearExample::LoginReq req;
    // req.set_account(1179953947);
    // req.set_password("bearPass");
    conn->Send(16, "1234", 4);
}

void onDisconnect(const TcpConnPtr& conn) {
    cout << "onDisconnect" << endl;
}

void onCmd16(const TcpConnPtr& conn, const std::shared_ptr<std::string>& msg) { 
    // cout << msg->account() <<endl;
    // cout << msg->password() << endl;
    cout << msg->data() << endl;
    conn->Send(17, nullptr, 0, 13, 14, 15, "123");
}

int main() {


    std::unique_ptr<Poller> poller(Poller::CreatePoller());
    std::unique_ptr<Codec> codec(new Codec());
    TcpServer server(poller.get(), codec.get());
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

        // activeChannelList.clear();
        // poller->Poll(true, false, activeChannelList, -1);
        // for (auto channel : activeChannelList) {
            // channel->HandleEvent();
        // }
    }
    return 0;
}