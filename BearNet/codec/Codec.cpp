#include "BearNet/codec/Codec.h"
#include "BearNet/Buffer.h"
#include "BearNet/tcp/TcpServer.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/message.h>

using namespace BearNet;

const std::string kCodecTag = "Bear";

void Codec::Encode(Buffer* buffer, uint16_t cmd, const void* data, int32_t dataSize) {
    buffer->Append(kCodecTag.c_str(), kCodecTagSize);
    auto pb = reinterpret_cast<const google::protobuf::Message*>(data);
    int byte_size = pb->ByteSize();
    buffer->AppendToNet(byte_size);
    buffer->AppendToNet(cmd);
    // buffer->Append(data, dataSize);
    // pb->SerializeToArray()
    pb->IsInitialized();
    uint8_t* start = reinterpret_cast<uint8_t*>(buffer->GetWritePtr());
    uint8_t* end = pb->SerializeWithCachedSizesToArray(start);
    if (end - start != byte_size) {
        printf("Error\n");
    }
    buffer->AppendReadIdx(byte_size);
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

    std::string msg = buffer->ReadString(size);
    // std::shared_ptr<google::protobuf::Message> message;
    // message->
    netPackage->cmd = cmd;
    netPackage->cmdMsg = msg;
    return 1;
}