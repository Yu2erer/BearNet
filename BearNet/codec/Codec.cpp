#include "BearNet/codec/Codec.h"
#include "BearNet/Buffer.h"

#include <iostream>

using namespace std;
using namespace BearNet;

const std::string kCodecTag = "Bear";

void Codec::Encode(Buffer* buffer, uint16_t cmd, const char* data, int32_t dataSize) {
    buffer->Append(kCodecTag.c_str(), kCodecTagSize);
    buffer->AppendToNet(dataSize);
    buffer->AppendToNet(cmd);
    buffer->Append(data, dataSize);
}
    

void Codec::Decode(const TcpConnPtr& conn, Buffer* buffer) {
    if (buffer->GetReadSize() < kCodecHeaderSize) {
        printf("rawSize < kCodecHeaderSize\n");
        return;
    }
    if (buffer->ReadString(kCodecTagSize) != kCodecTag) {
        printf("tag Failed.\n");
        // 应该关闭连接
        conn->ConnDestroyed();
        return;
    }
    cout << buffer->GetReadSize() << endl;
    int k = buffer->ReadInt32();
    printf("收到 长度 : %d\n", k);
    printf("收到 cmd: %d\n", buffer->ReadUint16());

    cout << buffer->ReadString(k) << endl;

}