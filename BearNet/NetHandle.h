#ifndef BEARNET_NETHANDLE_H
#define BEARNET_NETHANDLE_H

#include <vector>
#include <unordered_map>

#include "BearNet/base/Noncopyable.h"
#include "BearNet/tcp/TcpCommon.h"

namespace BearNet {

class NetHandle : private Noncopyable {
public:
    NetHandle() = default;
    virtual ~NetHandle() = default;
public:
    template <typename T>
    void Register(uint16_t cmd, const typename CmdCallBackT<T>::MessageCallBack& callBack) {
        if (m_cmdCallBack.find(cmd) != m_cmdCallBack.end()) {
            printf("Register: 已注册 %d\n", cmd);
            return;
        }
        std::shared_ptr<CmdCallBackT<T>> callBackPtr(new CmdCallBackT<T>(callBack));
        m_cmdCallBack[cmd] = std::move(callBackPtr);
    }
    void UnRegister(uint16_t cmd);
    const std::shared_ptr<CmdCallBack> GetCmdCallBack(uint16_t cmd);
    // 将所有注册的函数清除
    void Clear();
private:
    std::unordered_map<uint16_t, std::shared_ptr<CmdCallBack>> m_cmdCallBack;
};

}
#endif