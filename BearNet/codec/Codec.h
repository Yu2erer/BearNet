#ifndef BEARNET_CODEC_CODEC_H
#define BEARNET_CODEC_CODEC_H

#include "BearNet/tcp/TcpCommon.h"

namespace BearNet {

class Buffer;

template <typename... T>
struct Codec {
    Codec() = default;
    virtual ~Codec() = default;
    virtual int32_t Encode(Buffer* buffer, const void* data, int32_t dataSize, T... args) = 0;
    virtual int Decode(const TcpConnPtr& conn, const std::string& msg, const std::shared_ptr<void>& cmdCallBack) = 0;
};


}

#endif