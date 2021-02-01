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
    Client *targetClient;

    client->setCurrentCommand("PRIVMSG");
    
    // std::map<std::string, Client >::iterator it = this->sendClients.begin();
    // std::cout << "sendClients [ ";
    // for (; it != this->sendClients.end(); it++)
    //     std::cout << it->first << " ";
    // std::cout << "]" << std::endl;

    // std::map<std::string, Client *>::iterator it2 = this->serverList.begin();
    // std::cout << "serverList [ ";
    // for (; it2 != this->serverList.end(); it2++)
    //     std::cout << it2->first << " ";
    // std::cout << "]" << std::endl;

    // std::map<std::string, Client *>::iterator it3 = this->clientList.begin();
    // std::cout << "clientList [ ";
    // for (; it3 != this->clientList.end(); it3++)
    //     std::cout << it3->first << " ";
    // std::cout << "]" << std::endl;

    if (message.getParameters().size() == 0)        // 인자가 아무것도 없을 때 privmsg
        return ((this->*(this->replies[ERR_NORECIPIENT]))(message, client));
    else if (message.getParameters().size() == 1)   // 인자가 한개 있을 때 privmsg seun
        return ((this->*(this->replies[ERR_NOTEXTTOSEND]))(message, client));
    else if (message.getParameters().size() > 2 && message.getParameter(1).at(0) != ':')     // 인자가 2개 이상 일때 privmsg seun 1 2 3 4 
        return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
    else
    {   
        /* 클라이언트 일때
        먼저 같은 서버에서 확인을 하고 없다면
        다른 서버에 클라이언트를 확인
        */
        // clientList 비어 있을 수도 있음.
        
        // 같은 서버안에 있는 유저에게 보내는 경우
        if ((this->clientList.size() != 0) && (this->clientList.find(message.getParameter(0)) != this->clientList.end()))
        {   
            targetClient = this->clientList[message.getParameter(0)];
            // 클라이언트가 보냈을 때
            if (message.getPrefix().empty())
                this->sendMessage(Message(":" + getClientPrefix(client), "PRIVMSG", getTextMessage(message)), targetClient);
            // 서버가 보냈을 때
            else
                this->sendMessage(Message(message.getPrefix(), "PRIVMSG", getTextMessage(message)), targetClient);
        }
        // 같은 서버에 없어서 다른 서버로 보내는 경우
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
        
        // 첫번째 파라미터 맨 앞글자에 '#'이 들어오면 채널!
    }
    return (CONNECT);
}