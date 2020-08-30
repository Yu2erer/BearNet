#ifndef BEARNET_BASE_SINGLETON_H
#define BEARNET_BASE_SINGLETON_H

#include "BearNet/base/Noncopyable.h"

namespace BearNet {

template <typename T>
class Singleton : private Noncopyable {
public:
    static T& instance() {
        static T t;
        return t;
    }
protected:
    Singleton() {}
    virtual ~Singleton() {}
};

}

#endif