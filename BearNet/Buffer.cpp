#include <assert.h>
#include <sys/uio.h>

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

ssize_t Buffer::ReadFd(int fd) {
    char extra[65536];
    struct iovec vec[2];
    vec[0].iov_base = _Begin() + m_writeIndex;
    vec[0].iov_len = GetWriteSize();
    vec[1].iov_base = extra;
    vec[1].iov_len = sizeof(extra);
    const int iovcnt = (GetWriteSize() < sizeof(extra)) ? 2 : 1;
    const ssize_t n = ::readv(fd, vec, iovcnt);
    if (n < 0) {
        LogErr("Err: %d", n);
    } else if (static_cast<size_t>(n) <= GetWriteSize()) {
        m_writeIndex += n;
    } else {
        m_writeIndex = m_bufVec.size();
        _Extend(extra, n - GetWriteSize());
    }
    return n;
}

void Buffer::_Extend(const char* data, size_t size) {
    if (GetWriteSize() < size) {
        _MakeSpace(size);
    }
    assert(size <= GetWriteSize());
    std::copy(data, data + size, GetWritePtr());
    m_writeIndex += size;
}

void Buffer::_MakeSpace(size_t size) {
    if (GetWriteSize() + m_readIndex < size) {
        LogDebug("_MakeSpace 扩容  %d->%d", m_bufVec.size(), m_writeIndex + size);
        m_bufVec.resize(m_writeIndex + size);
    } else {
        LogDebug("_MakeSpace 挪动  %d->%d", m_bufVec.size(), m_writeIndex + size);
        size_t readSize = GetReadSize();
        std::copy(_Begin() + m_readIndex, _Begin() + m_writeIndex, _Begin());
        m_readIndex = 0;
        m_writeIndex = m_readIndex + readSize;
        assert(readSize == GetReadSize());
    }
}

