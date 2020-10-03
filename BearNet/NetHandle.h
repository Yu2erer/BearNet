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
    bool Register(uint16_t cmd, const typename CmdCallBackT<T>::MessageCallBack& callBack) {
        if (m_cmdCallBack.find(cmd) != m_cmdCallBack.end()) {
            return false;
        }
        std::shared_ptr<CmdCallBackT<T>> callBackPtr(new CmdCallBackT<T>(callBack));
        m_cmdCallBack[cmd] = std::move(callBackPtr);
        return true;
    }
    bool UnRegister(uint16_t cmd);
    const std::shared_ptr<void> GetCmdCallBack(uint16_t cmd);
    // 将所有注册的函数清除
    void Clear();
private:
    std::unordered_map<uint16_t, std::shared_ptr<void>> m_cmdCallBack;
};

}
#endif