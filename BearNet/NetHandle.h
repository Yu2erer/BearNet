#ifndef BEARNET_NETHANDLE_H
#define BEARNET_NETHANDLE_H

#include <vector>

#include "BearNet/base/Noncopyable.h"
#include "BearNet/tcp/TcpCommon.h"

namespace BearNet {
class NetHandle : private Noncopyable {
public:
    NetHandle() = default;
    virtual ~NetHandle() = default;
public:
    void Register(uint16_t cmd, CmdCallBack cmdCallBack);
    void UnRegister(uint16_t cmd);
    // 有可能为空 外界调用需要进行判断
    CmdCallBack& GetCmdCallBack(uint16_t cmd);
    // 将所有注册的函数清除
    void Clear();
private:
    class Impl {
    public:
        Impl();
        ~Impl();
    public:
        void Register(uint16_t cmd, CmdCallBack cmdCallBack);
        void UnRegister(uint16_t cmd);
        CmdCallBack& GetCmdCallBack(uint16_t cmd);
        void Clear();
    private:
        struct RegisterCmdInfo {
            CmdCallBack cmdCallBack;

            bool isRegister;
        };
        std::vector<RegisterCmdInfo> m_cmdCallBacks;
    };
    Impl impl;
};

}
#endif