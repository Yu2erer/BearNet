#include <iostream>
#include "BearNet/Channel.h"
#include "BearNet/poller/EpollPoller.h"
#include <sys/socket.h>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

using namespace BearNet;
using namespace std;

Channel* chan;
void acceptCallBack(Channel* channel, Poller* poller) {
    cout << "可读 有人连接了" << endl;
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof serv_addr);
    int connfd = accept(channel->Getfd(), (struct sockaddr*)&serv_addr, &addrlen);
    if (connfd < 0) {
        cout << "connfd < 0" << endl;
    }
    chan = new Channel(connfd, poller);
    chan->EnableReading();
    chan->SetReadCallBack([&connfd] {
        char buf[32];
        recv(connfd, buf, sizeof(buf), 0);
        cout << buf << endl;
        chan->DisableReading();
    });

}

int main() {
    int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(1234);
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(serv_sock, 20);
    int flags = fcntl(serv_sock, F_GETFL, 0);
    fcntl(serv_sock, F_SETFL, flags | O_NONBLOCK);

    Poller* poller = new EpollPoller();
    Channel channel(serv_sock, poller);
    channel.EnableReading();
    channel.SetReadCallBack(std::bind(&acceptCallBack, &channel, poller));


    
    Poller::ChannelList activeChannelList;
    for (;;) {
        activeChannelList.clear();
        poller->Poll(false, true, activeChannelList, 10000);
        for (auto channel : activeChannelList) {
            channel->HandleEvent();
        }

    }
    return 0;
}