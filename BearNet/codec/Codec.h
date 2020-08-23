#ifndef BEARNET_CODEC_CODEC_H
#define BEARNET_CODEC_CODEC_H

#include <cstdint>


namespace BearNet {

#pragma pack(push, 1)

struct DefaultNetPackHeader {
    char tag[4];  // 4 Bear
    uint32_t size; // 4
    uint16_t cmd; // 2
};

#pragma pack(pop)


}

#endif