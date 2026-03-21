#ifndef CHAT_CORE_H
#define CHAT_CORE_H

#include <string>

#include "types.h"


class ChatCore {
public:
    ChatCore(
        const std::string &clientName,
        const std::string &serverIp
    ) :
        m_clientName(clientName),
        m_serverIp(serverIp)
    {}
    ~ChatCore() = default;

    // Sends a message from this client to the server
    void SendMessage(const std::string &message);
    // Blocking function that waits for a message from the server
    std::string ReceiveMessage();

private:
    std::string m_clientName;
    std::string m_serverIp;

    std::string m_clientPublicKey;
    std::string m_clientPrivateKey;
};

#endif // CHAT_CORE_H
