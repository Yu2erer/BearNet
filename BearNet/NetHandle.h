#ifndef BEARNET_NETHANDLE_H
#define BEARNET_NETHANDLE_H

#include <vector>
#include <unordered_map>

#include "BearNet/base/Noncopyable.h"
#include "BearNet/tcp/TcpCommon.h"

#include <google/protobuf/message.h>

namespace BearNet {


class CallBack : private Noncopyable {
public:
    CallBack() = default;
    virtual ~CallBack() = default;
public:
    virtual void OnMessage(const TcpConnPtr& conn, const std::shared_ptr<google::protobuf::Message>& msg) const = 0;
    virtual std::shared_ptr<google::protobuf::Message> MakePtr() = 0;
};

template <typename T>
class CallBackT : public CallBack {
public:
    typedef std::function<void (const TcpConnPtr& conn, const std::shared_ptr<T>& msg)> MessageCallBack;
    typedef T ptrDef;
    CallBackT(const MessageCallBack& callBack) : m_callBack(callBack) {

    }
    // void operator()() {
    //     std::shared_ptr<T> other = static_pointer_cast<T>(msg);
    //     m_callBack(conn, other);
    // }
    void OnMessage(const TcpConnPtr& conn, const std::shared_ptr<google::protobuf::Message>& msg) const override {
        std::shared_ptr<T> other = std::static_pointer_cast<T>(msg);
        m_callBack(conn, other);
    }
    std::shared_ptr<google::protobuf::Message> MakePtr() override {
        std::shared_ptr<T> o(new T);
        return o;
    }
private:
    MessageCallBack m_callBack;
};

class NetHandle : private Noncopyable {
public:
    NetHandle() = default;
    virtual ~NetHandle() = default;
public:
    void Register(uint16_t cmd, CmdCallBack cmdCallBack);

    template <typename T>
    void Register2(uint16_t cmd, const typename CallBackT<T>::MessageCallBack& callBack) {
        impl.Register2<T>(cmd, callBack);
    }
    std::shared_ptr<CallBack> GetCmdCallBack2(uint16_t cmd) {
        return impl.GetCmdCallBack2(cmd);
    }
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
        template <typename T>
        void Register2(uint16_t cmd, const typename CallBackT<T>::MessageCallBack& callBack) {
            std::shared_ptr<CallBackT<T>> o(new CallBackT<T>(callBack));
            m_cmdCallBack2[cmd] = o;
        }
        void UnRegister(uint16_t cmd);
        CmdCallBack& GetCmdCallBack(uint16_t cmd);
        std::shared_ptr<CallBack> GetCmdCallBack2(uint16_t cmd) {
            return m_cmdCallBack2[cmd];
        }
        void Clear();
    private:
        struct RegisterCmdInfo {
            CmdCallBack cmdCallBack;
            bool isRegister;
        };
        std::vector<RegisterCmdInfo> m_cmdCallBacks;
        std::unordered_map<uint16_t, std::shared_ptr<CallBack>> m_cmdCallBack2;
    };
    Impl impl;
};

}
#endif