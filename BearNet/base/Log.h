#ifndef BEARNET_BASE_LOG_H
#define BEARNET_BASE_LOG_H

#include <cstring>
#include <cstdio>

namespace BearNet {


// Logger* g_logger;

#define __FILENAME__ (strrchr(__FILE__, '/') + 1)

#define LogFatal(format, ...) \
 printf("[%s][%s][%d]: " format "\n", __FILENAME__, __FUNCTION__,\
        __LINE__, ##__VA_ARGS__)

#define LogSysErr(format, ...) printf("[%s][%s][%d]: " format  "errno:%d "  "\n", __FILENAME__, __FUNCTION__,\
                            __LINE__, errno, ##__VA_ARGS__)

#define LogTrace(format, ...) printf("[%s][%s][%d]: " format "\n", __FILENAME__, __FUNCTION__,\
                            __LINE__, ##__VA_ARGS__)

#define LogErr(format, ...) printf("[%s][%s][%d]: " format "\n", __FILENAME__, __FUNCTION__,\
                            __LINE__, ##__VA_ARGS__)

#define LogDebug(format, ...) printf("[%s][%s][%d]: " format "\n", __FILENAME__, __FUNCTION__,\
                            __LINE__, ##__VA_ARGS__)

// #define LogError(format, ...) \
//     g_logger->WriteLogger();

// class Logger {
// public:
//   enum LogLevel {
//     TRACE,
//     DEBUG,
//     INFO,
//     ERROR,
//     NUM_LOG_LEVELS,
//   };
//     static void InitLogger();
//     void WriteLogger(LogLevel level, );
// };

}


#endif