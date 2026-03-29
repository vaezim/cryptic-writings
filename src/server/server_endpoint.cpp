#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QNetworkInterface>

#include "types.h"
#include "server_endpoint.h"


bool ServerEndpoint::initServer() {
    quint16 listeningPort = 9999;
    if (!listen(QHostAddress::Any, listeningPort)) {
        qFatal("Server::listen() failed.");
        close();
        return false;
    }

    // Find non-localhost IP address
    QString ipAddress;
    const QList<QHostAddress> &ipList = QNetworkInterface::allAddresses();
    for (const auto &ip : ipList) {
        // toIPv4Address() returns 0 for IPv6
        if (ip != QHostAddress::LocalHost && ip.toIPv4Address()) {
            ipAddress = ip.toString();
        }
    }
    if (ipAddress.isEmpty()) {
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    }
    INFO_LOG("Server is listening on " << ipAddress.toStdString() << ":" << serverPort());
    
    // If a new client connects, run handleNewClient()
    connect(this, &QTcpServer::newConnection,
            this, &ServerEndpoint::handleNewClient);

    return true;
}

void ServerEndpoint::handleNewClient() {
    QTcpSocket *clientSocket = nextPendingConnection();
    m_clientSockets.push_back(clientSocket);

    QHostAddress clientIPv4Address(clientSocket->peerAddress().toIPv4Address());
    GREEN_INFO_LOG("New client arrived from " << clientIPv4Address.toString().toStdString());

    // Release clientSocket if it gets disconnected
    connect(clientSocket, &QAbstractSocket::disconnected,
            clientSocket, &QObject::deleteLater);
}
