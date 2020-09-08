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
// 首先引入头文件
#include "BearNet/codec/Codec.h"
// 启用命名空间
using namespace BearNet;
// 继承 Codec 这个类, 在编码解码中进行自定义... 具体可见 example
template <typename... T>
struct RawCodec : Codec<T...> {
    int32_t Encode(Buffer* buffer, const void* data, int32_t dataSize, T... args) override {

    }

    int Decode(const TcpConnPtr& conn, Buffer* buffer, int32_t dataSize, const std::shared_ptr<void>& cmdCallBack) override {

    }
};
```

启动 tcpServer 😻

```c++
void onCmd16(const TcpConnPtr& conn, const std::shared_ptr<std::string>& msg) { 
    cout << msg->data() << endl;
}
int main() {
    // 绑定 codec
    auto codec = new RawCodec<>();
    std::unique_ptr<Poller> poller(Poller::CreatePoller());
    TcpServer server(poller.get(), codec);

    // 注册消息 std::string 为解码后的类型, 会自动转型作为 onCmd16的参数
    server.Register<std::string>(16, onCmd16);
    // 设置启动端口
    server.Start("0.0.0.0", 1234);
    
    Poller::ChannelList activeChannelList;

    for (;;) {
        activeChannelList.clear();
        poller->Poll(true, true, activeChannelList, -1);
        for (auto channel : activeChannelList) {
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