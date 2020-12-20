#include "Server.hpp"

static bool			isValidNickname(const Message &message)
{
	for (size_t i = 0; i < message.getParameter(0).length(); i++)
	{
		if (i == 0 && !isValidFormat(std::string(LETTER) + std::string(SPECIAL), message.getParameter(0)[i]))
			return false;
		else if (!isValidFormat(std::string(LETTER) + std::string(SPECIAL) + std::string(DIGIT), message.getParameter(0)[i]))
			return false;
	}
		return true;
}

static void			setNick(Client *client, const Message &message,
					std::map<std::string, Client> &sendClients,
					std::map<std::string, Client *> &clientList)
{
	if (client->getInfo(ORIGINNICK) == "")
		client->setInfo(ORIGINNICK, message.getParameter(0));
	else
	{
		sendClients.erase(client->getInfo(CURRENTNICK));
		clientList.erase(client->getInfo(CURRENTNICK));
	}
	client->setInfo(CURRENTNICK, message.getParameter(0));
	sendClients[message.getParameter(0)] = *client;
	clientList[message.getParameter(0)] = client;
}

int					Server::nickHandler(const Message &message, Client *client)
{
	if (client->getStatus() == UNKNOWN)
	{
		if (message.getParameters().empty())
			return ((this->*(this->replies[ERR_NONICKNAMEGIVEN]))(message, client));
		if (1 < message.getParameters().size())
			return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
		if (!isValidNickname(message) || 9 < message.getParameter(0).length())
			return ((this->*(this->replies[ERR_ERRONEUSNICKNAME]))(message, client));
		if (!client->getIsAuthorized())
		{
			this->sendMessage(Message("", ERROR_STR, std::string(":Access denied: Bad password")), client);
			return (DISCONNECT);
		}
		if (this->sendClients.find(message.getParameter(0)) != this->sendClients.end())
			return ((this->*(this->replies[ERR_NICKNAMEINUSE]))(message, client));
		setNick(client, message, this->sendClients, this->clientList);
		if (client->getInfo(USERNAME) == "")
		{
			this->sendMessage(Message("", message.getParameter(0), ":Nick registered"), client);
			return (CONNECT);
		}
		return ((this->*(this->replies[RPL_REGISTER_USER]))(message, client));
	}
	else if (client->getStatus() == USER)
	{
	}
	else if (client->getStatus() == SERVER)
	{
	}
	return (CONNECT);
}

