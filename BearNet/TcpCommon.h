#ifndef BEARNET_TCPCOMMON_H
#define BEARNET_TCPCOMMON_H

#include <memory>
#include <functional>

namespace BearNet {


class TcpConn;

typedef std::shared_ptr<TcpConn> TcpConnPtr;
typedef std::function<void (const TcpConnPtr, char*, int)> MessageCallBack;

}

#endif