#include "BearNet/NetHandle.h"

#include <cstdint>
#include <assert.h>

using namespace BearNet;

void NetHandle::Register(uint16_t cmd, CmdCallBack cmdCallBack) {
    impl.Register(cmd, std::move(cmdCallBack));
}

void NetHandle::UnRegister(uint16_t cmd) {
    impl.UnRegister(cmd);
}

CmdCallBack& NetHandle::GetCmdCallBack(uint16_t cmd) {
    return impl.GetCmdCallBack(cmd);
}

void NetHandle::Clear() {
    impl.Clear();
}

NetHandle::Impl::Impl() 
    : m_cmdCallBacks(UINT16_MAX + 1, RegisterCmdInfo()) {
    
}

NetHandle::Impl::~Impl() = default;

CmdCallBack& NetHandle::Impl::GetCmdCallBack(uint16_t cmd) {
    if (!m_cmdCallBacks[cmd].isRegister) {
        printf("GetCmdCallBack: 未注册 %d\n", cmd);
    }
    return m_cmdCallBacks[cmd].cmdCallBack;
}

void NetHandle::Impl::Register(uint16_t cmd, CmdCallBack cmdCallBack) {
    RegisterCmdInfo& info = m_cmdCallBacks[cmd];
    if (info.isRegister) {
        printf("Register: 已注册 %d\n", cmd);
        return;
    }
    info.cmdCallBack = std::move(cmdCallBack);
    info.isRegister = true;
}


void NetHandle::Impl::UnRegister(uint16_t cmd) {
    if (!m_cmdCallBacks[cmd].isRegister) {
        return;
    }
    m_cmdCallBacks[cmd].isRegister = false;
}

void NetHandle::Impl::Clear() {
    std::vector<RegisterCmdInfo> newCmdCallBacks(UINT16_MAX + 1, RegisterCmdInfo());
    m_cmdCallBacks.swap(newCmdCallBacks);
}

