#ifndef RAWCODEC_H
#define RAWCODEC_H

#include "BearNet/codec/Codec.h"

using namespace BearNet;

template <typename... T>
struct RawCodec : Codec<T...> {
    int32_t Encode(Buffer* buffer, const void* data, int32_t dataSize, T... args) override {
        printf("RawCodecEncode %ld\n", sizeof...(T));
        // _Encode(args...);
        return dataSize;
    }
    void _Encode(int a1, int a2, int a3) {
        printf("a1: %d, a2: %d, a3: %d\n", a1, a2, a3);
    }
    int Decode(const TcpConnPtr& conn, const std::string& msg, const std::shared_ptr<void>& cmdCallBack) override {
        printf("RawCodecDecode %ld\n", sizeof...(T));
        auto cb = std::static_pointer_cast<CmdCallBack<T...>>(cmdCallBack);
        auto ptr = cb->MakePtr();
        auto message = std::static_pointer_cast<std::string>(ptr);
        message->assign(msg);
        cb->OnMessage(conn, message);
        return 1;
    }
};

#endif