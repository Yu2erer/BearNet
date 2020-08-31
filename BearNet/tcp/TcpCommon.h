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
typedef std::function<void (const TcpConnPtr&, Buffer*)> MessageCallBack;
typedef std::function<void (const TcpConnPtr&)> CloseCallBack;

}

#endif