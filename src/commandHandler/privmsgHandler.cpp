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

    if (message.getParameters().size() == 0)        // 인자가 아무것도 없을 때 privmsg
        return ((this->*(this->replies[ERR_NORECIPIENT]))(message, client));
    else if (message.getParameters().size() == 1)   // 인자가 한개 있을 때 privmsg seun
        return ((this->*(this->replies[ERR_NOTEXTTOSEND]))(message, client));
    else if (message.getParameters().size() > 2)
        return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
    else
    {
        if (this->clientList.find(message.getParameter(0)) != this->clientList.end())
        {
            targetClient = this->clientList[message.getParameter(0)];
            this->sendMessage(Message(":" + getClientPrefix(client), "PRIVMSG", message.getParameter(0) + " :" + message.getParameter(1)), targetClient);
        }
    }

    return (CONNECT);
}