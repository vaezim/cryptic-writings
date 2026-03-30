#ifndef SERVER_ENDPOINT_H
#define SERVER_ENDPOINT_H

#include <memory>
#include <vector>

#include <QtNetwork/QTcpServer>


class ServerEndpoint : public QTcpServer {
public:
    ServerEndpoint() : QTcpServer() {}

    bool initServer();

private slots:
    void handleNewClient();
    void handleRead();

private:
    std::vector<QTcpSocket *> m_clientSockets;
    std::vector<std::unique_ptr<QDataStream>> m_dataStreams;
    std::unordered_map<QTcpSocket *, size_t> m_clientIndexMap;
};

#endif // SERVER_ENDPOINT_H
