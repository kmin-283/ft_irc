#include "Server.hpp"

int     Server::privmsgHandler(const Message &message, Client *client)
{
    client->setCurrentCommand("PRIVMSG");
    (void)message;
    std::cout << "client num = " << this->clientList.size() << std::endl;
    std::cout << "client nick = " << client->getInfo(NICK) << std::endl;
    return (CONNECT);
}