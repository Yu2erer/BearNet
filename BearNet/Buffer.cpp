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
        LogErr("Err: %d", n);
    } else if (static_cast<size_t>(n) <= writeSize) {
        m_writeIndex += n;
    } else {
        m_writeIndex = m_bufVec.size();
        LogDebug("收到总数 %d, 可用空间 %d, 需要追加 %d", n, writeSize, n - writeSize);
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

