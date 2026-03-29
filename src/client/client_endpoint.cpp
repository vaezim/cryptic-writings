#include "client_endpoint.h"


ClientEndpoint::ClientEndpoint(const ClientEndpointConfig &config) :
    m_clientName(config.clientName),
    m_serverIp(config.serverAddress),
    m_serverPort(config.serverPort) {
    m_serverSocket = new QTcpSocket(this);
    m_dataStream.setDevice(m_serverSocket);
    m_dataStream.setVersion(QDataStream::Qt_DefaultCompiledVersion);
    m_serverSocket->connectToHost(m_serverIp.c_str(), m_serverPort);
}

void ClientEndpoint::SendMessage(const std::string &message) {

}

std::string ClientEndpoint::ReceiveMessage() {
    return std::string();
}
