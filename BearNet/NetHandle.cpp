#include "BearNet/NetHandle.h"

#include <cstdint>
#include <assert.h>

using namespace BearNet;


void NetHandle::UnRegister(uint16_t cmd) {
    auto iter = m_cmdCallBack.find(cmd);
    if (iter == m_cmdCallBack.end()) {
        printf("UnRegister: 未注册 %d\n", cmd);
        return;
    }
    m_cmdCallBack.erase(iter);
}

const std::shared_ptr<CmdCallBack> NetHandle::GetCmdCallBack(uint16_t cmd) {
    if (m_cmdCallBack.find(cmd) == m_cmdCallBack.end()) {
        printf("GetCmdCallBack: 未注册 %d\n", cmd);
        return nullptr;
    }
    return m_cmdCallBack[cmd];
}

void NetHandle::Clear() {
    m_cmdCallBack.clear();
}

