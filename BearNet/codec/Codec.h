#ifndef BEARNET_CODEC_CODEC_H
#define BEARNET_CODEC_CODEC_H

#include <cstdint>
#include <string>
#include <cstring>

#include "BearNet/base/Noncopyable.h"
#include "BearNet/tcp/TcpConn.h"

#include <google/protobuf/message.h>

namespace BearNet {

class Buffer;

struct NetPackage {
    uint16_t cmd;
    std::string cmdMsg;
};

class Codec : private Noncopyable {
public:
    Codec() = default;
    ~Codec() = default;
public:
    void Encode(Buffer* buffer, uint16_t cmd, const void* data, int32_t dataSize);
    // -1 错误, 0 未完整, 1 解包成功
    int Decode(const TcpConnPtr& conn, Buffer* buffer, NetPackage* netPackage);
#pragma pack(push, 1)
struct CodecHeader {
    // tag: Bear
    char tag[4];
    int32_t size;
    uint16_t cmd;
};
#pragma pack(pop)

    static const int32_t kCodecTagSize = 4;
    static const int32_t kCodecHeaderSize = sizeof(CodecHeader);
};

}

#endif