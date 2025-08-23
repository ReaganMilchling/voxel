#include <cstring>

#ifndef __logger__
#define __logger__

#define _SHORT_FILE strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__
#define _VA_ARGS(...) , ##__VA_ARGS__;

#define NO_FMT      "%s\n"
#define FMT         "%-5s | %-20s | %15s:%-4d | "
#define ARGS(level) level, _SHORT_FILE, __FUNCTION__, __LINE__

#define PRINT(format, args...) fprintf(stderr, format, args);

//#define LOG(msg) std::cout << "INFO | " << strrchr(__FILE__, '/') << " | " << __FUNCTION__ << ":" << __LINE__ << " | " << msg << std::endl
#define LOG(msg) PRINT(FMT, ARGS("INFO")); std::cout << msg << std::endl;
#define LOGF(format, args...) PRINT(FMT, ARGS("INFO")); fprintf(stderr, format, args); fprintf(stderr, "\n");

#define LOG_ERROR(msg) PRINT(FMT, ARGS("ERROR")); std::cout << msg << std::endl;

#endif
