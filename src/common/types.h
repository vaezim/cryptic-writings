#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <chrono>
#include <iostream>


struct ClientEndpointConfig {
    std::string clientName;
    std::string serverAddress;
    int serverPort;
};

struct Message {
    std::chrono::system_clock::time_point time;
    std::string text;
    std::string clientName;
};

// Colors
#define COLOR_RED       "\x1B[91m"
#define COLOR_GREEN     "\x1B[92m"
#define COLOR_YELLOW    "\x1B[93m"
#define COLOR_DEFAULT   "\x1B[0m"

#define INFO_LOG(msg)                                   \
    do {                                                \
        std::cout <<                                    \
        "[INFO] " << msg << COLOR_DEFAULT << std::endl; \
    } while(0)

#define GREEN_INFO_LOG(msg)                             \
    do {                                                \
        std::cout << COLOR_GREEN                        \
        "[INFO] " << msg << COLOR_DEFAULT << std::endl; \
    } while(0)

#define WARNING_LOG(msg)                                    \
    do {                                                    \
        std::cout << COLOR_YELLOW                           \
        "[WARNING] " << msg << COLOR_DEFAULT << std::endl;  \
    } while(0)

#define ERROR_LOG(msg)                                      \
    do {                                                    \
        std::cout << COLOR_RED                              \
        "[RED] " << msg << COLOR_DEFAULT << std::endl;      \
    } while(0)

#endif // TYPES_H
