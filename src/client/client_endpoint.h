#ifndef CLIENT_ENDPOINT_H
#define CLIENT_ENDPOINT_H

#include <string>

#include <QtNetwork/QTcpSocket>

#include "types.h"


class ClientEndpoint : QObject {
public:
    ClientEndpoint(const ClientEndpointConfig &);
    ~ClientEndpoint() = default;

    std::string clientName() const { return m_clientName; }

    // Sends a message from client to server
    void SendMessage(const std::string &message);
    // Blocking function that waits for a message from server
    std::string ReceiveMessage();

private:
    std::string m_clientName;
    std::string m_serverIp;
    int m_serverPort;

    QTcpSocket *m_serverSocket;
    QDataStream m_dataStream;
};

#endif // CLIENT_ENDPOINT_H
