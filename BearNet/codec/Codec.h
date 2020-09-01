#ifndef BEARNET_CODEC_CODEC_H
#define BEARNET_CODEC_CODEC_H

#include <cstdint>
#include <string>
#include <cstring>

#include "BearNet/base/Noncopyable.h"
#include "BearNet/tcp/TcpConn.h"

namespace BearNet {

class Buffer;

class Codec : private Noncopyable {
public:
    Codec() = default;
    ~Codec() = default;
public:
    void Encode(Buffer* buffer, uint16_t cmd, const char* data, int32_t dataSize);
    void Decode(const TcpConnPtr& conn, Buffer* buffer);
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