#include "BearNet/NetHandle.h"

#include <cstdint>
#include <assert.h>

using namespace BearNet;


bool NetHandle::UnRegister(uint16_t cmd) {
    auto iter = m_cmdCallBack.find(cmd);
    if (iter == m_cmdCallBack.end()) {
        return false;
    }
    m_cmdCallBack.erase(iter);
    return true;
}

const std::shared_ptr<void> NetHandle::GetCmdCallBack(uint16_t cmd) {
    if (m_cmdCallBack.find(cmd) == m_cmdCallBack.end()) {
        return nullptr;
    }
    return m_cmdCallBack[cmd];
}

void NetHandle::Clear() {
    m_cmdCallBack.clear();
}

