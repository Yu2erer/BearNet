#include <assert.h>
#include <sys/uio.h>
#include <unistd.h>
#include <endian.h>
#include <cstdint>

#include "BearNet/base/Log.h"
#include "BearNet/Buffer.h"

using namespace BearNet;

void Buffer::Write(size_t size) {
    assert(size <= GetReadSize());
    if (size < GetReadSize()) {
        m_readIndex += size;
    } else {
        WriteAll();
    }
}

ssize_t Buffer::WriteFd(int fd) {
    const ssize_t n = ::write(fd, GetReadPtr(), GetReadSize());
    if (n > 0) {
        Write(n);
    }
    return n;
}

ssize_t Buffer::ReadFd(int fd) {
    const size_t writeSize = GetWriteSize();
    char extra[65536];
    struct iovec vec[2];
    vec[0].iov_base = _Begin() + m_writeIndex;
    vec[0].iov_len = writeSize;
    vec[1].iov_base = extra;
    vec[1].iov_len = sizeof(extra);
    const int iovcnt = (writeSize < sizeof(extra)) ? 2 : 1;
    const ssize_t n = ::readv(fd, vec, iovcnt);

    if (n < 0) {
        return n;
    } 
    
    if (static_cast<size_t>(n) <= writeSize) {
        m_writeIndex += n;
    } else {
        m_writeIndex = m_bufVec.size();
        LogTrace("收到总数 %d, 可用空间 %d, 需要追加 %d", n, writeSize, n - writeSize);
        Append(extra, n - writeSize);
    }
    return n;
}

void Buffer::Append(const char* data, size_t size) {
    if (GetWriteSize() < size) {
        _MakeSpace(size);
    }
    assert(size <= GetWriteSize());
    std::copy(data, data + size, GetWritePtr());
    m_writeIndex += size;
}

void Buffer::Append(const void* data, size_t size) {
    Append(static_cast<const char*>(data), size);
}

void Buffer::AppendToNet(int64_t x) {
    int64_t xbe64 = htobe64(x);
    Append(&xbe64, sizeof(xbe64));
}

void Buffer::AppendToNet(int32_t x) {
    int32_t xbe32 = htobe32(x);
    Append(&xbe32, sizeof(xbe32));

}

void Buffer::AppendToNet(int16_t x) {
    int16_t xbe16 = htobe16(x);
    Append(&xbe16, sizeof(xbe16));
}

void Buffer::AppendToNet(uint16_t x) {
    uint16_t xbe16 = htobe16(x);
    Append(&xbe16, sizeof(xbe16));
}

void Buffer::AppendToNet(int8_t x) {
    Append(&x, sizeof(x));
}

std::string Buffer::ReadString(size_t size) {
    assert(size <= GetReadSize());
    std::string result(GetReadPtr(), size);
    Write(size);
    return result;
}

int32_t Buffer::ReadInt32() {
    assert(sizeof(int32_t) <= GetReadSize());
    int32_t xbe32 = 0;
    ::memcpy(&xbe32, GetReadPtr(), sizeof(int32_t));
    Write(sizeof(int32_t));
    return be32toh(xbe32);
}

uint16_t Buffer::ReadUint16() {
    assert(sizeof(uint16_t) <= GetReadSize());
    uint16_t xbe16 = 0;
    ::memcpy(&xbe16, GetReadPtr(), sizeof(uint16_t));
    Write(sizeof(uint16_t));
    return be16toh(xbe16);
}

void Buffer::_MakeSpace(size_t size) {
    if (GetWriteSize() + m_readIndex < size) {
        LogTrace("_MakeSpace 扩容  %d->%d", m_bufVec.size(), m_writeIndex + size);
        m_bufVec.resize(m_writeIndex + size);
    } else {
        LogTrace("_MakeSpace 挪动  %d->%d", m_bufVec.size(), m_writeIndex + size);
        size_t readSize = GetReadSize();
        std::copy(_Begin() + m_readIndex, _Begin() + m_writeIndex, _Begin());
        m_readIndex = 0;
        m_writeIndex = m_readIndex + readSize;
        assert(readSize == GetReadSize());
    }
}
