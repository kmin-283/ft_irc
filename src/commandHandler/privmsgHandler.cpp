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
    else if (message.getParameters().size() > 2)    // 인자가 2개 이상 일때 privmsg seun 1 2 3 4 
        return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
    else
    {   
        /* 클라이언트 일때
        먼저 같은 서버에서 확인을 하고 없다면
        다른 서버에 클라이언트를 확인
        */
        // clientList 비어 있을 수도 있음.
        if ((this->clientList.size() != 0) && (this->clientList.find(message.getParameter(0)) != this->clientList.end()))
        {
            targetClient = this->clientList[message.getParameter(0)];
            this->sendMessage(Message(":" + getClientPrefix(client), "PRIVMSG", message.getParameter(0) + " :" + message.getParameter(1)), targetClient);
        }
        else
            return ((this->*(this->replies[ERR_NOSUCHNICK]))(message, client));
        
        // 첫번째 파라미터 맨 앞글자에 '#'이 들어오면 채널!
    }

    return (CONNECT);
}