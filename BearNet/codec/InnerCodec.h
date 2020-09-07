#ifndef BEARNET_CODEC_INNERCODEC_H
#define BEARNET_CODEC_INNERCODEC_H

#include <cstdint>
#include <string>
#include <cstring>

#include "BearNet/base/Noncopyable.h"
#include "BearNet/tcp/TcpConn.h"
#include "BearNet/codec/Codec.h"

namespace BearNet {

class Buffer;

// 内部使用
class InnerCodec : private Noncopyable {
public:
    template <typename... T>
    InnerCodec(Codec<T...>* codec) : m_ptrCodec(codec) { }
    ~InnerCodec() = default;
public:
    template <typename... T>
    void Encode(Buffer* buffer, uint16_t cmd, const void* data, int32_t dataSize, T... args) {
        buffer->Append(kCodecTag.c_str(), kCodecTagSize);

        auto codec = static_cast<Codec<T...>*>(m_ptrCodec);

        buffer->AppendToNet(0);
        buffer->AppendToNet(cmd);
        codec->Encode(buffer, data, dataSize, args...);
        // 补 size
    }
    // -1 错误, 0 未完整, 1 解包成功
    template <typename... T>
    int Decode(const TcpConnPtr& conn, Buffer* buffer) {
        if (buffer->GetReadSize() < kCodecHeaderSize) {
            return 0;
        }
        if (buffer->ReadString(kCodecTagSize) != kCodecTag) {
            printf("tag failed\n");
            return -1;
        }
        int32_t size = buffer->ReadInt32();
        uint16_t cmd = buffer->ReadUint16();
        printf("收到 长度: %d\n", size);
        printf("收到 cmd: %d\n", cmd);

        if (buffer->GetReadSize() < size) {
            printf("大小不对\n");
            return -1;
        }
        if (size > (UINT32_MAX - kCodecHeaderSize)) {
            printf("大小不对2\n");
            return -1;
        }

        std::string msg = buffer->ReadString(size);
        auto cmdCallBack = conn->GetCmdCallBack(cmd);
        if (!cmdCallBack) {
            printf("不认识 cmd: %d\n", cmd);
            return 1;
        }
        auto codec = static_cast<Codec<T...>*>(m_ptrCodec);
        int res = codec->Decode(conn, msg, cmdCallBack);
        return res;
    }

private:
    void* m_ptrCodec;
#pragma pack(push, 1)
struct CodecHeader {
    // tag: Bear
    char tag[4];
    int32_t size;
    uint16_t cmd;
};
#pragma pack(pop)
    const std::string kCodecTag = "Bear";
    static const int32_t kCodecTagSize = 4;
    static const int32_t kCodecHeaderSize = sizeof(CodecHeader);
};


}

#endif