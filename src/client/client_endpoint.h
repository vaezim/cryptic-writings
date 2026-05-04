#ifndef CLIENT_ENDPOINT_H
#define CLIENT_ENDPOINT_H

#include <string>
#include <vector>

#include <openssl/evp.h>
#include <QtNetwork/QTcpSocket>

#include "types.h"


class ClientEndpoint : public QObject {
    Q_OBJECT

public:
    ClientEndpoint(const ClientEndpointConfig &);
    ~ClientEndpoint();

    std::string clientName() const { return m_clientName; }

    QTcpSocket *serverSocket() const { return m_serverSocket; }
    QDataStream &dataStream() { return m_dataStream; }

    void sendMessage(const QString &message);

Q_SIGNALS:
    void receivedMessage(const QString &message);

private slots:
    void handleRead();

private:
    std::string m_clientName;
    std::string m_serverIp;
    int m_serverPort;

    QTcpSocket *m_serverSocket;
    QDataStream m_dataStream;

    QByteArray m_publicKey;
    QByteArray m_peerPublicKey;

    EVP_PKEY *m_privateKey{ nullptr };

    std::vector<unsigned char> m_sharedSecretKey;

    // Generate shared secret encryption key with
    // our private key and peer's public key
    void generateSharedSecretKey();

    // Generate public and private keys
    void generateAsymmetricKeyPair();

    void requestPeerPublicKey();
    QString decryptMessage(const QByteArray &encryptedMessage) const;
};

#endif // CLIENT_ENDPOINT_H
