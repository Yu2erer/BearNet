#include <assert.h>
#include <cstring>
#include <iostream>

#include "codec.h"

using namespace std;


DefaultCodec::DefaultCodec() 
    : m_headerSize(sizeof(DefaultNetPackHeader)), 
      m_tag("Bear") {
    m_recvPackage.header = &m_recvHeader;
}

DefaultCodec::~DefaultCodec() {

}

void DefaultCodec::Encode(uint16_t cmd, const void *msg, uint32_t msgSize, char **raw, uint32_t *rawSize) {
    if (msgSize > 0) {
        assert(msg != nullptr);
    }
    assert(raw != nullptr && rawSize != nullptr);
    if (msgSize + m_headerSize >= 1024 * 1024 * 2) {
        return;
    }
    memcpy(m_buffer + m_headerSize, msg, msgSize);
    _EncodeHeader(cmd, msgSize, raw, rawSize);
}

void DefaultCodec::_EncodeHeader(uint16_t cmd, uint32_t msgSize, char **raw, uint32_t* rawSize) {
    DefaultNetPackHeader header;
    memcpy(header.tag, m_tag.c_str(), sizeof(header.tag));
    header.cmd = cmd;
    memcpy(m_buffer, &header, m_headerSize);
    
    DefaultNetPackHeader* h = (DefaultNetPackHeader*)m_buffer;
    h->size = m_headerSize - sizeof(BaseNetPackHeader) + msgSize;

    cout << "encode headerSize: " << h->size << endl;

    *raw = m_buffer;
    *rawSize = m_headerSize + msgSize;
    cout << "encode rawSize: " << *rawSize << endl;
}


void DefaultCodec::Decode(const char *raw, uint32_t rawSize) {
    assert(raw != nullptr);

    if (rawSize < sizeof(BaseNetPackHeader)) {
        cout << "rawSize < sizeof(BaseNetPackHeader)" << endl;
        return;
    }
    BaseNetPackHeader* head = (BaseNetPackHeader*)raw;
    cout << "decode rawSize: " << rawSize << endl;
    cout << "m_headerSize: " << m_headerSize << endl;
    cout << "decode headerSize: " << head->size << endl;
    if (rawSize < sizeof(BaseNetPackHeader) + head->size) {
        cout << "rawSize < m_headerSize + head->size" << endl;

        return;
    }
    if (0 != memcmp(raw, m_tag.c_str(), m_tag.size())) {
        cout << "tag failed." << endl;
        return;
    }


    NetPackage& data = m_recvPackage;
    DefaultNetPackHeader& header = *(DefaultNetPackHeader*)m_recvPackage.header;
    uint32_t baseSize = sizeof(BaseNetPackHeader);
    memcpy(&header, raw, m_headerSize);
    data.cmd = header.cmd;
    data.rawSize = header.size + baseSize;
    data.msgRaw = raw + m_headerSize;
    data.msgRawSize = data.rawSize - m_headerSize;

    if (header.size < m_headerSize - baseSize) {
        printf("illegal client message, close connection\n");
        return;
    }

    cout << "Header.tag: " << header.tag << "; Header.size: "  << header.size << "; Header.cmd: " << header.cmd;
    cout << endl;
    string msg(data.msgRaw, data.msgRawSize);
    cout << "Msg: " << msg << endl;
}