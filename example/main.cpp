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
    BearExample::LoginReq req;
    req.set_account(1179953947);
    req.set_password("bearPass");
    conn->Send(16, (&req), 4);
}

void onDisconnect(const TcpConnPtr& conn) {
    cout << "onDisconnect" << endl;
}

void onCmd16(const TcpConnPtr& conn, const std::shared_ptr<BearExample::LoginReq>& msg) { 
    cout << "直接获取到" << endl;

    cout << msg->account() <<endl;
    cout << msg->password() << endl;

    // conn->Send(17);
}

void on16(const TcpConnPtr&, const std::string&) {

}
int main() {


    std::unique_ptr<Poller> poller(Poller::CreatePoller());

    TcpServer server(poller.get(), "0.0.0.0", 1234);
    server.SetConnectCallBack(onConnect);
    server.SetDisconnectCallBack(onDisconnect);
    server.Register(16, on16);
    server.Register2<BearExample::LoginReq>(16, onCmd16);

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