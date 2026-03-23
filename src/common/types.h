#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <chrono>


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

#endif // TYPES_H
