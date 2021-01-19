#include "Server.hpp"

/**
 * seunkim -> song 에게 보냄
 * 
 * :seunkim!~6@localhost PRIVMSG song :123
 * :보낸nick!~username@
 * 
 */

std::string     getClientPrefix(Client *client)
{
    std::string prefix;

    prefix += client->getInfo(NICK);
    prefix += std::string("!~");
    prefix += client->getInfo(USERNAME);
    prefix += std::string("@");
    prefix += client->getInfo(HOSTNAME);

    return (prefix);
}

int     Server::privmsgHandler(const Message &message, Client *client)
{
    Client *targetClient;

    client->setCurrentCommand("PRIVMSG");
    
    // clientList에서 이름 찾기
    if (this->clientList.find(message.getParameter(0)) != this->clientList.end())
    {
        targetClient = this->clientList[message.getParameter(0)];
        std::cout << "find! nick = " << message.getParameter(0) << std::endl;
        this->sendMessage(Message(":" + getClientPrefix(client), "PRIVMSG", message.getParameter(0) + " :" + message.getParameter(1)), targetClient);
        // std::cout << "hostname = " << targetClient->getInfo(HOSTNAME) << std::endl;
        // std::cout << "nick = " << targetClient->getInfo(NICK) << std::endl;
        // std::cout << "address = " << targetClient->getInfo(ADDRESS) << std::endl;
        // std::cout << "username = " << targetClient->getInfo(USERNAME) << std::endl;
        // std::cout << "realname = " << targetClient->getInfo(REALNAME) << std::endl;
    }

    return (CONNECT);
}