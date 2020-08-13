#ifndef BEARNET_BASE_NONCOPYABLE_H
#define BEARNET_BASE_NONCOPYABLE_H

namespace BearNet {


class Noncopyable {
public:
    Noncopyable(const Noncopyable&) = delete;
    void operator=(const Noncopyable&) = delete;

protected:
    Noncopyable() = default;
    ~Noncopyable() = default;

};


}

#endif