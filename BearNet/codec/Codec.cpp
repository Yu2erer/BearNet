#include "BearNet/codec/Codec.h"
#include "BearNet/Buffer.h"
#include "BearNet/tcp/TcpServer.h"

using namespace BearNet;

const std::string kCodecTag = "Bear";

void Codec::Encode(Buffer* buffer, uint16_t cmd, const char* data, int32_t dataSize) {
    buffer->Append(kCodecTag.c_str(), kCodecTagSize);
    buffer->AppendToNet(dataSize);
    buffer->AppendToNet(cmd);
    buffer->Append(data, dataSize);
}

int Codec::Decode(const TcpConnPtr& conn, Buffer* buffer, NetPackage* netPackage) {
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
    std::string* msg = new std::string;
    msg->assign(buffer->ReadString(size));
    netPackage->cmd = cmd;
    netPackage->cmdMsg.reset(msg);
    return 1;
}