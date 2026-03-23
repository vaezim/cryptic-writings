#ifndef CLIENT_ENDPOINT_H
#define CLIENT_ENDPOINT_H

#include <string>

#include "types.h"


class ClientEndpoint {
public:
    ClientEndpoint(
        const ClientEndpointConfig &config
    ) :
        m_clientName(config.clientName),
        m_serverIp(config.serverAddress),
        m_serverPort(config.serverPort)
    {}
    ~ClientEndpoint() = default;

    std::string clientName() const { return m_clientName; }
    std::string serverIp() const { return m_serverIp; }
    int serverPort() const { return m_serverPort; }

    // Sends a message from client to server
    void SendMessage(const std::string &message);
    // Blocking function that waits for a message from server
    std::string ReceiveMessage();

private:
    std::string m_clientName;
    std::string m_serverIp;
    int m_serverPort;
};

#endif // CLIENT_ENDPOINT_H
