#include "client_endpoint.h"


ClientEndpoint::ClientEndpoint(const ClientEndpointConfig &config) :
    m_clientName(config.clientName),
    m_serverIp(config.serverAddress),
    m_serverPort(config.serverPort) {
    m_serverSocket = new QTcpSocket(this);
    m_dataStream.setDevice(m_serverSocket);
    m_dataStream.setVersion(DATA_STREAM_PROTOCOL_VERSION);
    m_serverSocket->connectToHost(m_serverIp.c_str(), m_serverPort);
}

void ClientEndpoint::sendMessage(const QString &message) {
    m_dataStream << message;
}
