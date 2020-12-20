#include "Server.hpp"

static bool			isValidNickName(const Message &message)
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

static std::string	getHopCount(const Message &message)
{
	int					hopCount;
	std::stringstream	stream;
	std::string			hopCountStr;

	hopCountStr = std::string("1");
	if (message.getParameters().size() == 2)
	{
		stream << message.getParameter(1)
		.substr(1, message.getParameter(1).length());
		stream >> hopCount;
		stream << (++hopCount);
		hopCountStr = stream.str();
	}
	return (hopCountStr);
}

static void			setNick(Client *client, const Message &message,
					std::map<std::string, Client> &sendClients,
					std::map<std::string, Client *> &clientList)
{
	std::stringstream	stream;
	std::string			hopCount;

	if (client->getInfo(NICK) != "")
	{
		sendClients.erase(client->getInfo(NICK));
		clientList.erase(client->getInfo(NICK));
	}
	client->setInfo(HOPCOUNT, getHopCount(message));
	client->setInfo(NICK, message.getParameter(0));
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
		if (!isValidNickName(message) || 9 < message.getParameter(0).length())
			return ((this->*(this->replies[ERR_ERRONEUSNICKNAME]))(message, client));
		if (!client->getIsAuthorized())
			return ((this->*(this->replies[ERR_PASSUNAUTHORIE]))(message, client));
		if (this->sendClients.find(message.getParameter(0)) != this->sendClients.end())
			return ((this->*(this->replies[ERR_NICKNAMEINUSE]))(message, client));
		setNick(client, message, this->sendClients, this->clientList);
		if (client->getInfo(USERNAME) == "")
			return (CONNECT);
		(this->*(this->replies[RPL_REGISTERUSER]))(message, client);
		return ((this->*(this->replies[RPL_WELCOMEMESSAGE]))(message, client));
	}
	else if (client->getStatus() == USER)
	{
	}
	else if (client->getStatus() == SERVER)
	{
	}
	return (CONNECT);
}

static bool			isVaildUserName(const Message &message)
{
	for (size_t i = 0; i < message.getParameter(0).length(); i++)
	{
		if (isValidFormat(std::string(USER_FORMAT), message.getParameter(0)[i]))
			return false;
	}
	return true;
}

static void			setUser(const Message &message, Client *client, std::string address,
					std::map<std::string, Client> &sendClients, std::string serverName)
{
	std::string			realName;
	std::string			hopCount;

	realName = (*(message.getParameter(3).begin()) == ':'
	? message.getParameter(3).substr(1, message.getParameter(3).length())
	: message.getParameter(3));
	client->setInfo(USERNAME, message.getParameter(0));
	client->setInfo(HOSTNAME, serverName);
	client->setInfo(ADDRESS, address);
	client->setInfo(REALNAME, realName);
	if (client->getInfo(NICK) != "")
	{
		(sendClients[client->getInfo(NICK)]).setInfo(USERNAME, message.getParameter(0));
		(sendClients[client->getInfo(NICK)]).setInfo(HOSTNAME, serverName);
		(sendClients[client->getInfo(NICK)]).setInfo(ADDRESS, address);
		(sendClients[client->getInfo(NICK)]).setInfo(REALNAME, realName);
	}
}

int					Server::userHandler(const Message &message, Client *client)
{
	if (client->getStatus() == UNKNOWN)
	{
		if (message.getParameters().empty() || message.getParameters().size() != 4)
			return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
		if (!client->getIsAuthorized())
			return ((this->*(this->replies[ERR_PASSUNAUTHORIE]))(message, client));
		if (client->getInfo(USERNAME) != "")
			return ((this->*(this->replies[ERR_ALREADYREGISTRED]))(message, client));
		if (!isVaildUserName(message))
			return ((this->*(this->replies[ERR_ERRONEUSUSERNAME]))(message, client));
		setUser(message, client, this->ipAddress, this->sendClients, this->serverName);
		if (client->getInfo(NICK) == "")
			return (CONNECT);
		// TODO 유저 모드 관련 체크필요함
		(this->*(this->replies[RPL_REGISTERUSER]))(message, client);
		return ((this->*(this->replies[RPL_WELCOMEMESSAGE]))(message, client));
	}
	else if (client->getStatus() == SERVER)
	{
	}
	else if (client->getStatus() == USER)
	{
	}
	return (CONNECT);
}
