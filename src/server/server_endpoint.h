#ifndef SERVER_ENDPOINT_H
#define SERVER_ENDPOINT_H

#include <QtNetwork/QTcpServer>


class ServerEndpoint : public QTcpServer {
public:
    ServerEndpoint() : QTcpServer() {}

    bool initServer();

private slots:
    void handleNewClient();

private:
    std::vector<QTcpSocket *> m_clientSockets;
};

#endif // SERVER_ENDPOINT_H
