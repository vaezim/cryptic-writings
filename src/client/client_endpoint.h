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

    QTcpSocket *serverSocket() const { return m_serverSocket; }
    QDataStream &dataStream() { return m_dataStream; }

    void sendMessage(const QString &message);

private:
    std::string m_clientName;
    std::string m_serverIp;
    int m_serverPort;

    QTcpSocket *m_serverSocket;
    QDataStream m_dataStream;
};

#endif // CLIENT_ENDPOINT_H
