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

typedef std::function<void (const TcpConnPtr&, const std::shared_ptr<void>&)> CmdCallBack;

}

#endif