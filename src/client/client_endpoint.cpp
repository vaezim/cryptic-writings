#include <chrono>
#include <stdexcept>

#include <openssl/pem.h>
#include <openssl/sha.h>
#include <openssl/rand.h>

#include <QtCore/QTimer>
#include <QtCore/QCryptographicHash>

#include "client_endpoint.h"


ClientEndpoint::ClientEndpoint(const ClientEndpointConfig &config) :
    m_clientName(config.clientName),
    m_serverIp(config.serverAddress),
    m_serverPort(config.serverPort)
{
    // Initialize client socket and connect to the server
    m_serverSocket = new QTcpSocket(this);
    m_dataStream.setDevice(m_serverSocket);
    m_dataStream.setVersion(DATA_STREAM_PROTOCOL_VERSION);
    m_serverSocket->connectToHost(m_serverIp.c_str(), m_serverPort);

    // X25519 key pair
    generateAsymmetricKeyPair();

    // Send public key to the server
    m_dataStream << REGISTER_PUBLIC_KEY << m_publicKey;

    requestPeerPublicKey();

    // Release clientSocket if it gets disconnected
    connect(m_serverSocket, &QAbstractSocket::disconnected,
            m_serverSocket, &QObject::deleteLater);

    // Signals when new server message has arrived
    connect(m_serverSocket, &QIODevice::readyRead,
        this, &ClientEndpoint::handleRead);
}

ClientEndpoint::~ClientEndpoint() {
    if (m_privateKey) EVP_PKEY_free(m_privateKey);
}

void ClientEndpoint::sendMessage(const QString &message) {
    if (m_sharedSecretKey.empty()) {
        ERROR_LOG("Shared secret key is not generated yet. Can't encrypt messages.");
        return;
    }

    QByteArray plainText = message.toUtf8();

    EVP_CIPHER_CTX *context = EVP_CIPHER_CTX_new();

    // Random 12-byte IV (initialization vector). Unique for every message.
    unsigned char iv[AES_256_GCM_IV_SIZE];
    RAND_bytes(iv, AES_256_GCM_IV_SIZE);

    // Initialize AES-256-GCM
    EVP_EncryptInit_ex(context, CIPHER_TYPE, nullptr, nullptr, nullptr);
    EVP_EncryptInit_ex(context, nullptr, nullptr, m_sharedSecretKey.data(), iv);

    int cipherTextSize{ 0 };
    QByteArray cipherText;
    cipherText.resize(plainText.size());

    int writeSize;
    EVP_EncryptUpdate(context,
        (unsigned char *)cipherText.data(), &writeSize,
        (const unsigned char *)plainText.data(), plainText.size());
    cipherTextSize += writeSize;

    EVP_EncryptFinal_ex(context, (unsigned char *)cipherText.data(), &writeSize);
    cipherTextSize += writeSize;

    // 16-bytes tag used to verify that the message has not been modified.
    unsigned char tag[AES_256_GCM_TAG_SIZE];
    EVP_CIPHER_CTX_ctrl(context, EVP_CTRL_GCM_GET_TAG, AES_256_GCM_TAG_SIZE, tag);

    EVP_CIPHER_CTX_free(context);

    // Construct the final encrypted message
    QByteArray encryptedMessage;
    encryptedMessage.append((char *)iv, AES_256_GCM_IV_SIZE);
    encryptedMessage.append((char *)tag, AES_256_GCM_TAG_SIZE);
    encryptedMessage.append(cipherText);
    m_dataStream << ENCRYPTED_MESSAGE << encryptedMessage;
}

void ClientEndpoint::handleRead() {
    while (!m_dataStream.atEnd()) {
        m_dataStream.startTransaction();

        int _type;
        m_dataStream >> _type;
        MessageType messageType = static_cast<MessageType>(_type);

        switch (messageType) {
            case SENDING_PEER_PUBLIC_KEY: {
                QByteArray data;
                m_dataStream >> data;
                if (!m_dataStream.commitTransaction()) {
                    return;
                }
                m_peerPublicKey = data;
                generateSharedSecretKey();
                break;
            }

            case PEER_PUBLIC_KEY_NOT_READY: {
                if (!m_dataStream.commitTransaction()) {
                    return;
                }
                QTimer::singleShot(std::chrono::seconds(1), this, &ClientEndpoint::requestPeerPublicKey);
                break;
            }

            case ENCRYPTED_MESSAGE: {
                QByteArray data;
                m_dataStream >> data;
                if (!m_dataStream.commitTransaction()) {
                    return;
                }
                QString message = decryptMessage(data);
                emit receivedMessage(message);
                break;
            }

            default: {
                m_dataStream.rollbackTransaction();
                return;
            }
        }
    }
}

void ClientEndpoint::generateSharedSecretKey() {
    if (m_peerPublicKey.isEmpty()) {
        ERROR_LOG("Failed to generate shared secret key. Peer's public key is empty.");
        return;
    }

    const unsigned char *data =
        reinterpret_cast<const unsigned char *>(m_peerPublicKey.data());
    EVP_PKEY *peerPublicKey = d2i_PUBKEY(nullptr, &data, m_peerPublicKey.size());
    if (!peerPublicKey) {
        ERROR_LOG("Failed to decode peer's public key. d2i_PUBKEY() returned null.");
        return;
    }

    EVP_PKEY_CTX *context = EVP_PKEY_CTX_new(m_privateKey, nullptr);
    if (!context) {
        ERROR_LOG("Failed to generate shared secret key. EVP_PKEY_CTX_new() returned null.");
        return;
    }

    if (EVP_PKEY_derive_init(context) <= 0) {
        EVP_PKEY_CTX_free(context);
        ERROR_LOG("Failed to generate shared secret key. EVP_PKEY_derive_init() <= 0");
        return;
    }

    if (EVP_PKEY_derive_set_peer(context, peerPublicKey) <= 0) {
        EVP_PKEY_CTX_free(context);
        ERROR_LOG("Failed to generate shared secret key. EVP_PKEY_derive_set_peer() <= 0");
        return;
    }

    size_t secretKeySize;
    if (EVP_PKEY_derive(context, nullptr, &secretKeySize) <= 0) {
        EVP_PKEY_CTX_free(context);
        ERROR_LOG("Failed to generate shared secret key. EVP_PKEY_derive() <= 0");
        return;
    }

    std::vector<unsigned char> rawSecretKey(secretKeySize);
    if (EVP_PKEY_derive(context, rawSecretKey.data(), &secretKeySize) <= 0) {
        EVP_PKEY_CTX_free(context);
        ERROR_LOG("Failed to generate shared secret key. EVP_PKEY_derive() <= 0");
        return;
    }

    m_sharedSecretKey.resize(SHA256_DIGEST_LENGTH);
    SHA256(rawSecretKey.data(), secretKeySize, m_sharedSecretKey.data());

    // Display the first 10 digits of secret key's hash in hex format.
    // Two clients should compare these to make sure they are identical,
    // otherwise MITM attack might have happened.
    QByteArray secretKey((const char *)m_sharedSecretKey.data(), m_sharedSecretKey.size());
    QByteArray secretKeyHash = QCryptographicHash::hash(secretKey, QCryptographicHash::Sha256);
    QString fingerprint = secretKeyHash.toHex().left(10).toUpper();
    fingerprint.insert(5, ' ');
    QString fingerprintMessage = QString(
        "This is the fingerprint of the SHARED SECRET KEY between you and your peer.\n"
        "Compare them and make sure you both have the same fingerprint. If not, close the connection!\n\n"
        "%1"
    ).arg(fingerprint);
    emit receivedMessage(fingerprintMessage);

    EVP_PKEY_CTX_free(context);
}

void ClientEndpoint::generateAsymmetricKeyPair() {
    EVP_PKEY_CTX *privateKeyContext = EVP_PKEY_CTX_new_id(NID_X25519, nullptr);
    if (!privateKeyContext) {
        throw std::runtime_error("Failed to create new asymmetric key context.");
    }
    if (EVP_PKEY_keygen_init(privateKeyContext) <= 0) {
        EVP_PKEY_CTX_free(privateKeyContext);
        throw std::runtime_error("Failed to initialize asymmetric key context.");
    }
    if (EVP_PKEY_keygen(privateKeyContext, &m_privateKey) <= 0) {
        EVP_PKEY_CTX_free(privateKeyContext);
        throw std::runtime_error("Failed to generate private key.");
    }
    EVP_PKEY_CTX_free(privateKeyContext);

    GREEN_INFO_LOG("--- Public Key ---");
    PEM_write_PUBKEY(stdout, m_privateKey);

    // Convert public key to byte array
    unsigned char *publicKey{ nullptr };
    int publicKeySize = i2d_PUBKEY(m_privateKey, &publicKey);
    if (publicKeySize <= 0) {
        throw std::runtime_error("Failed to convert public key to bytes.");
    }
    m_publicKey = QByteArray(reinterpret_cast<const char *>(publicKey), publicKeySize);
    OPENSSL_free(publicKey);
}

void ClientEndpoint::requestPeerPublicKey() {
    m_dataStream << GET_PEER_PUBLIC_KEY;
}

QString ClientEndpoint::decryptMessage(const QByteArray &encryptedMessage) const {
    if (encryptedMessage.size() < AES_256_GCM_IV_SIZE + AES_256_GCM_TAG_SIZE) {
        WARNING_LOG("Length of encrypted message must be at least 28 (IV len + TAG len)");
        return QString();
    }

    // Pointers to IV, tag, and data
    const unsigned char *iv = (const unsigned char *)encryptedMessage.data();
    const unsigned char *tag =
        (const unsigned char *)encryptedMessage.data() + AES_256_GCM_IV_SIZE;
    const unsigned char *cipherText =
        (const unsigned char *)encryptedMessage.data() + AES_256_GCM_IV_SIZE + AES_256_GCM_TAG_SIZE;

    int cipherTextSize = encryptedMessage.size() - AES_256_GCM_IV_SIZE - AES_256_GCM_TAG_SIZE;
    int plainTextSize{ 0 };
    QByteArray plainText;
    plainText.resize(cipherTextSize);

    EVP_CIPHER_CTX *context = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(context, CIPHER_TYPE, nullptr, nullptr, nullptr);
    EVP_DecryptInit_ex(context, nullptr, nullptr, m_sharedSecretKey.data(), iv);

    int writeSize;
    EVP_DecryptUpdate(context,
        (unsigned char *)plainText.data(), &writeSize,
        (const unsigned char *)cipherText, cipherTextSize);
    plainTextSize += writeSize;

    // Verify message integrity with tag
    EVP_CIPHER_CTX_ctrl(context, EVP_CTRL_GCM_SET_TAG, AES_256_GCM_TAG_SIZE, (void *)tag);
    int integrityCheck = EVP_DecryptFinal_ex(context,
        (unsigned char *)plainText.data() + plainTextSize, &writeSize);

    EVP_CIPHER_CTX_free(context);

    if (integrityCheck <= 0) {
        ERROR_LOG("Message integrity check with tag failed. EVP_CIPHER_CTX_ctrl() returned " << integrityCheck);
        return QString();
    }

    plainTextSize += writeSize;
    plainText.resize(plainTextSize);
    return QString::fromUtf8(plainText);
}
