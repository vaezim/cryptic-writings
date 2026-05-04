#include <stdexcept>

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QNetworkInterface>

#include "types.h"
#include "server_endpoint.h"


bool ServerEndpoint::startServer() {
    constexpr quint16 listeningPort = 9999;
    if (!listen(QHostAddress::Any, listeningPort)) {
        close();
        throw std::runtime_error("Server::listen() failed.");
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
    if (m_clients.first.socket != nullptr && m_clients.second.socket != nullptr) {
        WARNING_LOG("2 Clients are already connected. Cannot serve more clients.");
        return;
    }

    QTcpSocket *clientSocket = nextPendingConnection();
    auto dataStreamPtr = std::make_unique<QDataStream>(clientSocket);
    dataStreamPtr->setVersion(DATA_STREAM_PROTOCOL_VERSION);
    if (m_clients.first.socket == nullptr) {
        m_clients.first.socket = clientSocket;
        m_clients.first.dataStream = std::move(dataStreamPtr);
    } else {
        m_clients.second.socket = clientSocket;
        m_clients.second.dataStream = std::move(dataStreamPtr);
    }

    QHostAddress clientIPv4Address(clientSocket->peerAddress().toIPv4Address());
    GREEN_INFO_LOG("New client arrived from " << clientIPv4Address.toString().toStdString());

    // Release clientSocket if it gets disconnected
    connect(clientSocket, &QAbstractSocket::disconnected,
            clientSocket, &QObject::deleteLater);

    // Signals when new client message has arrived
    connect(clientSocket, &QIODevice::readyRead,
            this, &ServerEndpoint::handleRead);
}

void ServerEndpoint::handleRead() {
    QTcpSocket *senderSocket = qobject_cast<QTcpSocket *>(sender());
    ClientId clientId = UNKNOWN;
    if (senderSocket == m_clients.first.socket) {
        clientId = CLIENT_1;
    } else if (senderSocket == m_clients.second.socket) {
        clientId = CLIENT_2;
    } else {
        ERROR_LOG("Sender socket ignored as it does not belong to either client 1 or 2");
        return;
    }

    auto &dataStream = (clientId == CLIENT_1) ?
        m_clients.first.dataStream : m_clients.second.dataStream;

    while (!dataStream->atEnd()) {
        dataStream->startTransaction();

        int _type;
        (*dataStream) >> _type;
        MessageType messageType = static_cast<MessageType>(_type);

        switch (messageType) {
            case REGISTER_PUBLIC_KEY: {
                QByteArray data;
                (*dataStream) >> data;
                if (!dataStream->commitTransaction()) {
                    return;
                }
                if (clientId == CLIENT_1) {
                    m_clients.first.publicKey = data;
                } else {
                    m_clients.second.publicKey = data;
                }
                break;
            }

            case GET_PEER_PUBLIC_KEY: {
                if (!dataStream->commitTransaction()) {
                    return;
                }
                const QByteArray &key = (clientId == CLIENT_1) ?
                    m_clients.second.publicKey : m_clients.first.publicKey;
                if (key.isEmpty()) {
                    (*dataStream) << PEER_PUBLIC_KEY_NOT_READY;
                    break;
                }
                (*dataStream) << SENDING_PEER_PUBLIC_KEY << key;
                break;
            }

            case ENCRYPTED_MESSAGE: {
                QByteArray message;
                (*dataStream) >> message;
                if (!dataStream->commitTransaction()) {
                    return;
                }
                auto &peerDataStream = (clientId == CLIENT_1) ?
                    m_clients.second.dataStream : m_clients.first.dataStream;
                (*peerDataStream) << ENCRYPTED_MESSAGE << message;
                break;
            }

            default: {
                dataStream->rollbackTransaction();
                return;
            }
        }
    }
}
