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
    void Encode(Buffer* buffer, uint16_t cmd, const void* data, int32_t dataSize);
    // -1 错误, 0 未完整, 1 解包成功
    int Decode(const TcpConnPtr& conn, Buffer* buffer);
private:
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
/*
编解码 不应该用模板实现, 毕竟要限制死 编解码的参数个数, 如果不一致 可能会导致解码失败
如何让用户去扩展 Codec呢?
Encode:

*/
}

#endif