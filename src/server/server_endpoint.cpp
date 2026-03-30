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
    m_clientIndexMap[clientSocket] = m_clientSockets.size();
    m_clientSockets.push_back(clientSocket);

    auto dataStreamPtr = std::make_unique<QDataStream>(clientSocket);
    dataStreamPtr->setVersion(DATA_STREAM_PROTOCOL_VERSION);
    m_dataStreams.push_back(std::move(dataStreamPtr));

    QHostAddress clientIPv4Address(clientSocket->peerAddress().toIPv4Address());
    GREEN_INFO_LOG("New client arrived from " << clientIPv4Address.toString().toStdString());

    // Release clientSocket if it gets disconnected
    connect(clientSocket, &QAbstractSocket::disconnected,
            clientSocket, &QObject::deleteLater);

    // Signals when new client data has arrived
    connect(clientSocket, &QIODevice::readyRead,
            this, &ServerEndpoint::handleRead);
}

void ServerEndpoint::handleRead() {
    QTcpSocket *sendingClient = qobject_cast<QTcpSocket *>(sender());
    size_t sendingClientIndex = m_clientIndexMap[sendingClient];

    // Read the message sent by sendingClient
    auto &sendingDataStream = m_dataStreams[sendingClientIndex];
    sendingDataStream->startTransaction();
    QString message;
    (*sendingDataStream) >> message;
    if (!sendingDataStream->commitTransaction()) {
        return;
    }

    // Broadcast the message to all other clients
    for (size_t i{ 0 }; i < m_clientSockets.size(); i++) {
        if (i == sendingClientIndex) {
            continue;
        }
        auto &receivingDataStream = m_dataStreams[i];
        (*receivingDataStream) << message;
    }
}
