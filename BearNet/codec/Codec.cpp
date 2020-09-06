#include "BearNet/codec/Codec.h"
#include "BearNet/Buffer.h"
#include "BearNet/tcp/TcpServer.h"

#include <google/protobuf/message.h>

using namespace BearNet;

const std::string kCodecTag = "Bear";

void Codec::Encode(Buffer* buffer, uint16_t cmd, const void* data, int32_t dataSize) {
    buffer->Append(kCodecTag.c_str(), kCodecTagSize);
    int byte_size = 0;
    // const google::protobuf::Message* pb = nullptr;
    // Encode tag, cmd
    // Encode GetDataSize
    // Encode new things

    // decode tag, cmd, size
    // decode new things
    // if (data) {
    //     pb = reinterpret_cast<const google::protobuf::Message*>(data);
    //     byte_size = pb->ByteSizeLong();
    // }

    buffer->AppendToNet(dataSize);
    buffer->AppendToNet(cmd);
    buffer->Append(data, dataSize);
    // if (data) {
    //     uint8_t* start = reinterpret_cast<uint8_t*>(buffer->GetWritePtr());
    //     uint8_t* end = pb->SerializeWithCachedSizesToArray(start);
    //     if (end - start != byte_size) {
    //         printf("Error\n");
    //     }
    //     buffer->AppendReadIndex(byte_size);
    // }
}

int Codec::Decode(const TcpConnPtr& conn, Buffer* buffer) {
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
    } else {
        auto ptr = cmdCallBack->MakePtr();
        auto message = std::static_pointer_cast<std::string>(ptr);
        // auto message = std::static_pointer_cast<google::protobuf::Message>(ptr);
        // message->Clear();
        // message->ParseFromString(msg);
        // cmdCallBack->OnMessage(conn, message);
        message->assign(msg);
        cmdCallBack->OnMessage(conn, message);
    }
    return 1;
}