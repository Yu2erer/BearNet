#include <assert.h>
#include <sys/uio.h>
#include <unistd.h>

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

ssize_t Buffer::WriteFd() {
    const ssize_t n = ::write(m_fd, GetReadPtr(), GetReadSize());
    if (n > 0) {
        Write(n);
    }
    return n;
}

ssize_t Buffer::ReadFd() {
    const size_t writeSize = GetWriteSize();
    char extra[65536];
    struct iovec vec[2];
    vec[0].iov_base = _Begin() + m_writeIndex;
    vec[0].iov_len = writeSize;
    vec[1].iov_base = extra;
    vec[1].iov_len = sizeof(extra);
    const int iovcnt = (writeSize < sizeof(extra)) ? 2 : 1;
    const ssize_t n = ::readv(m_fd, vec, iovcnt);

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

