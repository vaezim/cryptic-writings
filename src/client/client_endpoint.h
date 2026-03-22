#ifndef CLIENT_ENDPOINT_H
#define CLIENT_ENDPOINT_H

#include <string>

#include "types.h"


class ClientEndpoint {
public:
    ClientEndpoint(
        const std::string &clientName,
        const std::string &serverIp,
        int serverPort
    ) :
        m_clientName(clientName),
        m_serverIp(serverIp),
        m_serverPort(serverPort)
    {}
    ~ClientEndpoint() = default;

    // Sends a message from this client to the server
    void SendMessage(const std::string &message);
    // Blocking function that waits for a message from the server
    std::string ReceiveMessage();

private:
    std::string m_clientName;
    std::string m_serverIp;
    int m_serverPort;
};

#endif // CLIENT_ENDPOINT_H
