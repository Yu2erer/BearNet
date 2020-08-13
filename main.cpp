#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#include "BearNet/Channel.h"
#include "BearNet/poller/EpollPoller.h"
#include "BearNet/Acceptor.h"

using namespace BearNet;
using namespace std;

Channel* chan;
char buf[1024];

Poller* poller = new EpollPoller();

void readCallBack() {
    memset(&buf, 0, 1024);
    do {
        ssize_t ret = recv(chan->Getfd(), buf, 20, MSG_DONTWAIT);
        if (ret > 0) {
            cout << buf << endl;
            send(chan->Getfd(), buf, strlen(buf), 0);
            return;
        }
        if (ret == 0) {
            cout << "已关闭" << endl;
            chan->Remove();
            SocketHelper::Close(chan->Getfd());
            delete chan;
            chan = nullptr;
        }
        if (errno == EAGAIN) {
            cout << "Error: err: 1  " << errno << endl;
            return;
        } else {
            cout << "Error: err: 2  " << errno << endl;
            return;
        }
        
    } while (true);
    
}

void acceptCallBack(int sock) {
    cout << "可读 有人连接了" << endl;
    chan = new Channel(sock, poller);
    chan->EnableReading();
    chan->SetReadCallBack(std::bind(&readCallBack));
    chan->SetErrorCallBack([] {
        cout << "Error" <<endl;
        chan->Remove();
    });
    chan->SetCloseCallBack([] {
        cout << "Close" <<endl;
        chan->Remove();
    });
    ::write(sock, "How are you?\n", 13);


}

int main() {
    Acceptor acceptor(poller, "0.0.0.0", 1234);
    acceptor.SetNewConnectionCallBack(acceptCallBack);
    acceptor.Listen();
    
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