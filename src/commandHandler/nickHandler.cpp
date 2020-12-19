#include "Server.hpp"

static bool			checkStr(std::string str, std::string::iterator iterator)
{
	if (str.find(*iterator) != std::string::npos)
		return (true);
	return (false);
}

static bool			checkNickForm(std::string nick)
{
	std::string::iterator iterator;

	iterator = nick.begin();
	if (!checkStr(std::string(LETTER) + std::string(SPECIAL), iterator))
		return (false);
	while (++iterator != nick.end())
		if (!checkStr(std::string(LETTER) + std::string(SPECIAL) + std::string(DIGIT), iterator))
			return (false);
	return (true);
}

int					Server::nickHandler(const Message &message, Client *client)
{
	std::string		userNick;

	userNick = client->getInfo(CURRENTNICK) == "" ? "*" : client->getInfo(CURRENTNICK);
	if (client->getStatus() == UNKNOWN)
	{
		if (message.getParameters().empty())
			return ((this->*(this->replies[ERR_NONICKNAMEGIVEN]))(message, client));
		if (1 < message.getParameters().size())
			return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
		if (!checkNickForm(message.getParameter(0)) || 9 < message.getParameter(0).length())
			return ((this->*(this->replies[ERR_ERRONEUSNICKNAME]))(message, client));
		if (!client->getIsAuthorized())
		{
			this->sendMessage(Message("", ERROR_STR, std::string(":Access denied: Bad password")), client);
			return (DISCONNECT);
		}
		if (this->sendClients.find(message.getParameter(0)) != this->sendClients.end())
			return ((this->*(this->replies[ERR_NICKNAMEINUSE]))(message, client));
		if (client->getInfo(ORIGINNICK) == "")
			client->setInfo(ORIGINNICK, message.getParameter(0));
		else
		{
			this->sendClients.erase(client->getInfo(CURRENTNICK));
			this->clientList.erase(client->getInfo(CURRENTNICK));
		}
		client->setInfo(CURRENTNICK, message.getParameter(0));
		this->sendClients[message.getParameter(0)] = *client;
		this->clientList[message.getParameter(0)] = client;
		if (client->getInfo(USERNAME) == "")
		{
			this->sendMessage(Message("", message.getParameter(0), ":Nick registered"), client);
			return (CONNECT);
		}
		return ((this->*(this->replies[RPL_REGISTER_USER]))(message, client));

		/*
			환영 메시지
			NICK broead cast
			USER broad cast
		*/
	}
	else if (client->getStatus() == USER)
	{
		// 프리픽스

	}
	else if (client->getStatus() == SERVER)
	{
	}
	return (CONNECT);
}

