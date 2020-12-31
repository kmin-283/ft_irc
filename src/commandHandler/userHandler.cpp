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

static int			setNick(Client *client, const Message &message, bool isServer,
					std::map<std::string, Client> &sendClients,
					std::map<std::string, Client *> &clientList)
{
	std::stringstream	stream;

	if (client->getInfo(NICK) != "")
	{
		sendClients.erase(client->getInfo(NICK));
		if (!isServer)
			clientList.erase(client->getInfo(NICK));
	}
	if (client->getStatus() == UNKNOWN)
		client->setInfo(HOPCOUNT, getHopCount(message));
	client->setInfo(NICK, message.getParameter(0));
	sendClients[message.getParameter(0)] = *client;
	if (!isServer)
		clientList[message.getParameter(0)] = &sendClients[message.getParameter(0)];
	return (CONNECT);
}

int					Server::setLocalNick(const Message &message, Client *client)
{
	if (message.getParameters().empty())
		return ((this->*(this->replies[ERR_NONICKNAMEGIVEN]))(message, client));
	if (1 != message.getParameters().size())
		return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
	if (!isValidNickName(message) || 9 < message.getParameter(0).length())
		return ((this->*(this->replies[ERR_ERRONEUSNICKNAME]))(message, client));
	if (!client->getIsAuthorized())
		return ((this->*(this->replies[ERR_PASSUNAUTHORIE]))(message, client));
	if (this->sendClients.count(message.getParameter(0))
	|| this->serverName == message.getParameter(0))
		return ((this->*(this->replies[ERR_NICKNAMEINUSE]))(message, client));
	setNick(client, message, false, this->sendClients, this->clientList);
	if (client->getInfo(USERNAME) == "")
		return (CONNECT);
	(this->*(this->replies[RPL_REGISTERUSER]))(message, client);
	return ((this->*(this->replies[RPL_WELCOMEMESSAGE]))(message, client));
}

int					Server::resetLocalNick(const Message &message, Client *client)
{
	if (message.getPrefix() != ""
	&& message.getPrefix() != std::string(":") + client->getInfo(SERVERNAME))
		return ((this->*(this->replies[ERR_PREFIX]))(message, client));
	if (message.getParameters().size() != 1)
		return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
	if (!isValidNickName(message) || 9 < message.getParameter(0).length())
		return ((this->*(this->replies[ERR_ERRONEUSNICKNAME]))(message, client));
	if (this->sendClients.count(message.getParameter(0))
	|| this->serverName == message.getParameter(0))
		return ((this->*(this->replies[ERR_NICKNAMEINUSE]))(message, client));
	(this->*(this->replies[RPL_NICK]))(message, client);
	(this->*(this->replies[RPL_NICKBROADCAST]))(message, client);
	return (setNick(client, message, false, this->sendClients, this->clientList));
}

int					Server::localNickHandler(const Message &message, Client *client)
{
	if (client->getStatus() == UNKNOWN)
		return (this->setLocalNick(message, client));
	else if (client->getStatus() == USER)
		return (this->resetLocalNick(message, client));
	return (CONNECT);
}

int					Server::setRemoteNick(const Message &message, Client *client)
{
	Client	remoteUser(client->getFd(),true);

	if (message.getParameters().size() != 2)
		return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
	if (this->sendClients.count(message.getParameter(0))
	|| this->serverName == message.getParameter(0))
		(this->*(this->replies[RPL_KILL]))(message, client);
	if (this->clientList.count(message.getParameter(0)))
		return ((this->*(this->replies[ERR_NICKCOLLISION]))(message, this->clientList[message.getParameter(0)]));
	if (this->sendClients[message.getParameter(0)].getStatus() == USER)
	{
		this->sendClients.erase(message.getParameter(0));
		return (CONNECT);
	}
	if (this->sendClients[message.getParameter(0)].getStatus() == SERVER
	|| this->serverName == message.getParameter(0))
		return ((this->*(this->replies[ERR_CANTKILLSERVER]))(message, client));
	setNick(&remoteUser, message, true, this->sendClients, this->clientList);
	return (CONNECT);
}

int					Server::remoteNickHandler(const Message &message, Client *client)
{
	if (message.getPrefix() == "")
		return (this->setRemoteNick(message, client));
	// TODO :dakim NICK :de 처리 필요
	return ((this->*(this->replies[ERR_PREFIX]))(message, client));
}

int					Server::nickHandler(const Message &message, Client *client)
{
	if (client->getStatus() == SERVER)
		return (this->remoteNickHandler(message, client));
	return (this->localNickHandler(message, client));
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
	client->setInfo(USERNAME, message.getParameter(0));
	client->setInfo(HOSTNAME, serverName);
	client->setInfo(ADDRESS, address);
	client->setInfo(REALNAME, message.getParameter(3));
	if (client->getInfo(NICK) != "")
	{
		(sendClients[client->getInfo(NICK)]).setInfo(USERNAME, message.getParameter(0));
		(sendClients[client->getInfo(NICK)]).setInfo(HOSTNAME, serverName);
		(sendClients[client->getInfo(NICK)]).setInfo(ADDRESS, address);
		(sendClients[client->getInfo(NICK)]).setInfo(REALNAME, message.getParameter(3));
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
	else if (client->getStatus() == USER)
	{
		if (message.getPrefix() != ""
		&& message.getPrefix() != std::string(":") + client->getInfo(SERVERNAME))
			return ((this->*(this->replies[ERR_PREFIX]))(message, client));
		return ((this->*(this->replies[ERR_ALREADYREGISTRED]))(message, client));
	}
	else if (client->getStatus() == SERVER)
	{
	}
	return (CONNECT);
}
