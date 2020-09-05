#ifndef BEARNET_BUFFER_H
#define BEARNET_BUFFER_H

#include <vector>
#include <sys/types.h>
#include <string>

namespace BearNet {

class Buffer {
public:
    explicit Buffer(size_t initSize = 1024) 
        : m_bufVec(initSize), m_readIndex(0),
          m_writeIndex(0) {
     }
    ~Buffer() = default;
public:
    ssize_t ReadFd(int fd);
    ssize_t WriteFd(int fd);
public:
    void Write(size_t size);
    void WriteAll() { m_readIndex = m_writeIndex = 0; }
    // Change ReadIndex
    void AppendWriteIndex(size_t size);
    // Change WriteIndex
    void AppendReadIndex(size_t size);
public:
    void Append(const char* data, size_t size);
    void Append(const void* data, size_t size);
    void Append(const std::string& data);
    // Host Endian To Net Endian
    void AppendToNet(int64_t x);
    void AppendToNet(uint64_t x);
    void AppendToNet(int32_t x);
    void AppendToNet(uint32_t x);
    void AppendToNet(int16_t x);
    void AppendToNet(uint16_t x);
    void AppendToNet(int8_t x);
    void AppendToNet(uint8_t x);
public:
    std::string PeekString(size_t size);
    std::string ReadString(size_t size);
    int32_t PeekInt32();
    int32_t ReadInt32();
    uint16_t PeekUint16();
    uint16_t ReadUint16();
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
    size_t m_readIndex;
    size_t m_writeIndex;
};

}

#endif