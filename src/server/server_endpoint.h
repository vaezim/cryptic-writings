#ifndef SERVER_ENDPOINT_H
#define SERVER_ENDPOINT_H

#include <memory>
#include <utility>

#include <QtNetwork/QTcpServer>


struct Client_s {
    QTcpSocket *socket{ nullptr };
    std::unique_ptr<QDataStream> dataStream;
    QByteArray publicKey;
};

class ServerEndpoint : public QTcpServer {
public:
    ServerEndpoint() : QTcpServer() {}

    bool startServer();

private slots:
    void handleNewClient();
    void handleRead();

private:
    // Only 2 clients are supported
    std::pair<Client_s, Client_s> m_clients;
};

#endif // SERVER_ENDPOINT_H
