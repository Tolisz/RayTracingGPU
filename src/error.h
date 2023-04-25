#ifndef ERROR_MESSAGE_MACRO
#define ERROR_MESSAGE_MACRO

#include <iostream>

#define ERROR(msg)                                                                  \
    do                                                                              \
    {                                                                               \
        std::cerr << "[ERROR][" << __FILE__ << ":" << __LINE__ << "] " << msg << '\n';     \
        exit(1);                                                                    \
    }                                                                               \
    while (0)

#define WARNING(msg)                                                                    \
    do                                                                                  \
    {                                                                                   \
        std::cerr << "[WARNING][" << __FILE__ << ":" << __LINE__ << "] " << msg << '\n';       \
    }                                                                                   \
    while (0)


#endif