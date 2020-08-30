#ifndef BEARNET_BUFFER_H
#define BEARNET_BUFFER_H

#include <vector>
#include <sys/types.h>

namespace BearNet {

class Buffer {
public:
    explicit Buffer(size_t initSize, const int fd) 
        : m_bufVec(initSize),
          m_fd(fd) { }
    ~Buffer() { }
public:
    ssize_t ReadFd();
    ssize_t WriteFd();
public:
    void Write(size_t size);
    void WriteAll() { m_readIndex = m_writeIndex = 0; }
    void Append(const char* data, size_t size);
public:
    char* GetWritePtr() { return _Begin() + m_writeIndex; }
    const char* GetWritePtr() const { return _Begin() + m_writeIndex; }
    char* GetReadPtr() { return _Begin() + m_readIndex; }
    const char* GetReadPtr() const { return _Begin() + m_readIndex; }
public:
    size_t GetSize() const { return _BufSize(); }
    size_t GetReadSize() const { return m_writeIndex - m_readIndex; }
    size_t GetWriteSize() const { return _BufSize() - m_writeIndex; }
private:
    char* _Begin() { return m_bufVec.data(); }
    const char* _Begin() const { return m_bufVec.data(); }
    size_t _BufSize() const { return m_bufVec.size(); }
    void _MakeSpace(size_t size);
private:
    std::vector<char> m_bufVec;
    const int m_fd;
    size_t m_readIndex;
    size_t m_writeIndex;
};

}

#endif