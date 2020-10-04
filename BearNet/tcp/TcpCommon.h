#ifndef BEARNET_TCPCOMMON_H
#define BEARNET_TCPCOMMON_H

#include <memory>
#include <functional>

namespace BearNet {


class TcpConn;
class Buffer;

typedef std::shared_ptr<TcpConn> TcpConnPtr;
typedef std::function<void (const TcpConnPtr&)> ConnectCallBack;
typedef std::function<void (const TcpConnPtr&)> DisconnectCallBack;
typedef std::function<void (const TcpConnPtr&, Buffer*)> InnerMessageCallBack;
typedef std::function<void (const TcpConnPtr&)> InnerCloseCallBack;


template <typename... PARAMS>
class CmdCallBack : private Noncopyable {
public:
    CmdCallBack() = default;
    virtual ~CmdCallBack() = default;
public:
    virtual void OnMessage(const TcpConnPtr& conn, const std::shared_ptr<void>& msg, PARAMS... args) const = 0;
    virtual std::shared_ptr<void> MakePtr() = 0;
};

template <typename T, typename... PARAMS>
class CmdCallBackT : public CmdCallBack<PARAMS...> {
public:
    using MessageCallBack = std::function<void (const TcpConnPtr& conn, const std::shared_ptr<T>& msg, PARAMS... args)>;

    CmdCallBackT(const MessageCallBack& callBack) : m_callBack(callBack) {}

    void OnMessage(const TcpConnPtr& conn, const std::shared_ptr<void>& msg, PARAMS... args) const override {
        std::shared_ptr<T> other = std::static_pointer_cast<T>(msg);
        m_callBack(conn, other, args...);
    }
    std::shared_ptr<void> MakePtr() override {
        std::shared_ptr<T> ptr(new T);
        return ptr;
    }
private:
    MessageCallBack m_callBack;
};

template <typename... PARAMS>
using EncodeFunc = std::function<int (Buffer* buffer, const void* data, int dataSize, PARAMS...)>;

template <typename... PARAMS>
using DecodeFunc = std::function<int (const TcpConnPtr&, Buffer*, int dataSize, const std::shared_ptr<CmdCallBack<PARAMS...>>&)>;

}



#endif