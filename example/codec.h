#ifndef CODEC_H
#define CODEC_H

#include <cstdint>
#include <string>

#include "BearNet/base/Singleton.h"

struct NetPackage {
    uint16_t cmd;
    void* header;
    const char* msgRaw;
    // 消息长度
    uint32_t msgRawSize;
    // 头 + 消息总长
    uint32_t rawSize;
};

class DefaultCodec : public BearNet::Singleton<DefaultCodec> {
public:
    DefaultCodec();
    ~DefaultCodec();
public:
    void Encode(uint16_t cmd, const void *msg, uint32_t msgSize, char **raw, uint32_t* rawSize);
    void Decode(const char *raw, uint32_t rawSize);
private:
    void _EncodeHeader(uint16_t cmd, uint32_t msgSize, char **raw, uint32_t* rawSize);
private:
#pragma pack(push, 1)
struct BaseNetPackHeader {
    char tag[4];  // 4 Bear
    uint32_t size; // 4
};
struct DefaultNetPackHeader : BaseNetPackHeader{
    uint16_t cmd; // 2
};

#pragma pack(pop)
    char m_buffer[1024 * 1024 * 2];
    uint32_t m_headerSize;
    const std::string m_tag;
    NetPackage m_recvPackage;
    BaseNetPackHeader m_recvHeader;
};

#endif