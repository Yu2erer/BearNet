#ifndef RAWCODEC_H
#define RAWCODEC_H

#include "BearNet/codec/Codec.h"

using namespace BearNet;

template <typename... T>
struct RawCodec : Codec<T...> {
    int32_t Encode(Buffer* buffer, const void* data, int32_t dataSize, T... args) override {
        buffer->Append(data, dataSize);
        return dataSize;
    }

    int Decode(const TcpConnPtr& conn, Buffer* buffer, int32_t dataSize, const std::shared_ptr<void>& cmdCallBack) override {
        auto callBack = std::static_pointer_cast<CmdCallBack<T...>>(cmdCallBack);
        auto ptr = callBack->MakePtr();
        auto message = std::static_pointer_cast<std::string>(ptr);
        auto msg = buffer->ReadString(dataSize);
        message->assign(msg);
        callBack->OnMessage(conn, message);
        return 1;
    }
};

#endif