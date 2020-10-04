# 🐻 BearNet

<!-- <p align="center"> -->
<a href="https://github.com/Yu2erer/BearNet/actions?query=workflow%3Abuild"><img src="https://github.com/Yu2erer/BearNet/workflows/build/badge.svg?branch=master"></a>
<!-- </p> -->

🐻 目前是基于 epoll 的 一个 单线程 游戏网络库, 目前只支持 Linux.

编译方式非常简单

```sh
git clone https://github.com/Yu2erer/BearNet.git
cmake .
make
```

即会在 bin 目录下编译出 示例的 example 可执行文件 `rawServer`

## How to use? 👀
首先你需要自定义你的编解码器, 即 codec

```c++
int Encode(Buffer* buffer, const void* data, int dataSize) {
    buffer->Append(data, dataSize);
    return dataSize;
}

int Decode(const TcpConnPtr& conn, Buffer* buffer, int dataSize, const std::shared_ptr<CmdCallBack<>>& callBack) {
    auto ptr = callBack->MakePtr();
    auto message = std::static_pointer_cast<std::string>(ptr);
    auto msg = buffer->ReadString(dataSize);
    message->assign(msg);
    callBack->OnMessage(conn, message);
    return 1;
}
```

启动 tcpServer 😻

```c++
void onCmd16(const TcpConnPtr& conn, const std::shared_ptr<std::string>& msg) { 
    cout << msg->data() << endl;
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
```

## Todo 😹
* 继续完善接口
* 添加基于Protobuf的Example
* 将日志打印接口暴露
* 加入应用层心跳包
* 将各个配置接口暴露(例如缓冲区大小)
* 功能 & 边界 & 压力测试
* 支持 Poll
* 支持 macOS, Windows
* 支持 UDP
* 支持多线程