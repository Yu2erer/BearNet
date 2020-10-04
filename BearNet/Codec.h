#ifndef BEARNET_CODEC_H
#define BEARNET_CODEC_H

#include <cstdint>
#include <string>
#include <cstring>

#include "BearNet/base/Noncopyable.h"
#include "BearNet/tcp/TcpConn.h"

namespace BearNet {

class Buffer;

template <typename... PARAMS>
class Codec : private Noncopyable {
public:
    Codec() = default;
    ~Codec() = default;
public:
    void Encode(Buffer* buffer, uint16_t cmd, const void* data, int dataSize, PARAMS... args) {
        buffer->Append(kCodecTag.c_str(), kCodecTagSize);

        char* writePtr = buffer->GetWritePtr();
        buffer->AddWriteIndex(sizeof(int));
        buffer->AppendToNet(cmd);
        int newSize = m_encodeFunc(buffer, data, dataSize, args...);
        // 补 size
        buffer->PrependToNet(writePtr, newSize);
    }
    // -1 错误, 0 未完整, 1 解包成功
    int Decode(const TcpConnPtr& conn, Buffer* buffer) {
        if (buffer->GetReadSize() < kCodecHeaderSize) {
            return 0;
        }
        std::string tag(buffer->ReadString(kCodecTagSize));
        if (tag != kCodecTag) {
            printf("tag failed %s\n", tag.c_str());
            return -1;
        }
        int size = buffer->ReadInt32();
        uint16_t cmd = buffer->ReadUint16();
        printf("收到 长度: %d\n", size);
        printf("收到 cmd: %d\n", cmd);

        if (buffer->GetReadSize() < static_cast<uint32_t>(size)) {
            printf("大小不对\n");
            return -1;
        }
        if (size > (INT32_MAX - kCodecHeaderSize)) {
            printf("大小不对2\n");
            return -1;
        }

        auto cmdCallBack = conn->GetCmdCallBack(cmd);
        
        // 没注册也要清掉缓冲区 不能影响下一个包
        if (!cmdCallBack) {
            printf("不认识 cmd: %d\n", cmd);
            buffer->Write(size);
            return 1;
        }
        auto callBack = std::static_pointer_cast<CmdCallBack<PARAMS...>>(cmdCallBack);

        return m_decodeFunc(conn, buffer, size, callBack);
    }
public:
    void SetEncodeFunc(const EncodeFunc<PARAMS...>& func) {
        m_encodeFunc = func;
    }
    void SetDecodeFunc(const DecodeFunc<PARAMS...>& func) {
        m_decodeFunc = func;
    }
private:
    EncodeFunc<PARAMS...> m_encodeFunc;
    DecodeFunc<PARAMS...> m_decodeFunc;
#pragma pack(push, 1)
struct CodecHeader {
    // tag: Bear
    char tag[4];
    int size;
    uint16_t cmd;
};
#pragma pack(pop)
    const std::string kCodecTag = "Bear";
    static const int kCodecTagSize = 4;
    static const int kCodecHeaderSize = sizeof(CodecHeader);
};


}

#endif