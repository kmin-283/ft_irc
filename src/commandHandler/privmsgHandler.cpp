#include "Server.hpp"

/**
 * seunkim -> song 에게 보냄
 * 
 * :seunkim!~6@localhost PRIVMSG song :123
 * :보낸nick!~username@
 * 
 */

// PRIVMSG 이후에 보낼 메시지 리턴
std::string     getTextMessage(const Message &message)
{
    std::string textMessage;

    textMessage += message.getParameter(0);
    
    if (message.getParameter(1).at(0) == ':')
    {
        for (int i = 1; i < int(message.getParameters().size()); i++)
            textMessage += (" " + message.getParameter(i));
    }
    else
        textMessage += (" :" + message.getParameter(1));
    return (textMessage);
}

// getUserPrefix랑 같은 함수
std::string     Server::getClientPrefix(Client *client)
{
	std::string returnString;

	returnString += client->getInfo(NICK);
	returnString += std::string("!~");
	returnString += client->getInfo(USERNAME);
	returnString += std::string("@");
	returnString += client->getInfo(HOSTNAME);
	return (returnString);
}

int     Server::privmsgHandler(const Message &message, Client *client)
{
    Client                  *sender;
    Client                  *targetClient;
    Channel                 *targetChannel;
    std::vector<Client *>   joinedUsers;

    client->setCurrentCommand("PRIVMSG");
    
    if (message.getParameters().size() == 0)        // 인자가 아무것도 없을 때 privmsg
        return ((this->*(this->replies[ERR_NORECIPIENT]))(message, client));
    else if (message.getParameters().size() == 1)   // 인자가 한개 있을 때 privmsg seun
        return ((this->*(this->replies[ERR_NOTEXTTOSEND]))(message, client));
    else if (message.getParameters().size() > 2 && message.getParameter(1).at(0) != ':')     // 인자가 2개 이상 일때 privmsg seun 1 2 3 4 
        return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
    else
    {   
        // privmsg #my hello
        if (message.getParameter(0).at(0) == '#' || message.getParameter(0).at(0) == '&')
        {   
            if (this->localChannelList.find(message.getParameter(0)) != this->localChannelList.end())
                targetChannel = &this->localChannelList[message.getParameter(0)];
            else if (this->remoteChannelList.find(message.getParameter(0)) != this->remoteChannelList.end())
                targetChannel = &this->remoteChannelList[message.getParameter(0)];
            else    // 401
                return ((this->*(this->replies[ERR_NOSUCHNICK]))(message, client));

            if (client->getStatus() == USER)
                sender = client;
            else
                sender = &this->sendClients[message.getPrefix().substr(1)];

            // 지금 채널에 있는 유저들에게 메세지 보냄
            // :kmin!~q@localhost PRIVMSG #my :hello
            joinedUsers = targetChannel->getUsersList(this->serverName);
            for (int i = 0; i < (int)joinedUsers.size(); i++)
                this->sendMessage(Message(":" + getClientPrefix(sender), "PRIVMSG", getTextMessage(message)), joinedUsers[i]);

            // 다른 서버에 메시지 보냄
            // :seunkim PRIVMSG #my :hello 
            this->broadcastMessage(Message(":" + sender->getInfo(NICK), "PRIVMSG", getTextMessage(message)), client);
        }
        else
        {
            if ((this->clientList.size() != 0) && (this->clientList.find(message.getParameter(0)) != this->clientList.end()))
            {       
                targetClient = this->clientList[message.getParameter(0)];
                if (message.getPrefix().empty())
                    this->sendMessage(Message(":" + getClientPrefix(client), "PRIVMSG", getTextMessage(message)), targetClient);
                else
                    this->sendMessage(Message(message.getPrefix(), "PRIVMSG", getTextMessage(message)), targetClient);
            }
            else if (this->sendClients.find(message.getParameter(0)) != this->sendClients.end())
            {
                targetClient = &this->sendClients[message.getParameter(0)];
                if (message.getPrefix().empty())
                    this->sendMessage(Message(":" + getClientPrefix(client), "PRIVMSG", getTextMessage(message)), targetClient);
                else
                    this->sendMessage(Message(message.getPrefix(), "PRIVMSG", getTextMessage(message)), targetClient);
            }
            else
                return ((this->*(this->replies[ERR_NOSUCHNICK]))(message, client));
            }
    }
    return (CONNECT);
}

// 채널 모드가 n 이고 그 채널에 없을 때
// :irc.example.net 404 kmin #my :Cannot send to channel