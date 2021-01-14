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

static int			setNick(Client *client, const Message &message, bool isServer,
					std::map<std::string, Client> &sendClients,
					std::map<std::string, Client *> &clientList)
{
	if (client->getInfo(NICK) != "")
	{
		sendClients.erase(client->getInfo(NICK));
		if (!isServer)
			clientList.erase(client->getInfo(NICK));
	}
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
	if (!isValidNickName(message) || 9 < message.getParameter(0).length())
		return ((this->*(this->replies[ERR_ERRONEUSNICKNAME]))(message, client));
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


static Message		getMessage(const Message &message)
{
	// std::stringstream	stream;
	std::string			parameters;
	Message				returnMessage;

	parameters = message.getParameter(0).substr(1, message.getParameter(0).length());
	parameters += std::string(" :1");
	returnMessage = Message(std::string(""), RPL_NICK, parameters);
	return (returnMessage);
}


int					Server::resetRemoteNick(const Message &message, Client *client)
{
	std::string			prefix;
	Message				formatedMessage;
	Client				remoteUser;

	prefix = message.getPrefix().substr(1, message.getPrefix().length());
	if (this->sendClients[prefix].getStatus() != UNKNOWN
	&& this->sendClients[prefix].getStatus() != USER)
		return ((this->*(this->replies[ERR_PREFIX]))(message, client));
	if (message.getParameters().size() != 1)
		return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
	if (message.getParameter(0)[0] != ':')
		return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
	formatedMessage = getMessage(message);
	remoteUser = this->sendClients[prefix];
	if (!isValidNickName(formatedMessage) || 9 < formatedMessage.getParameter(0).length())
		return ((this->*(this->replies[ERR_ERRONEUSNICKNAME]))(formatedMessage, client));
	if (this->sendClients.count(formatedMessage.getParameter(0))
	|| this->serverName == formatedMessage.getParameter(0))
		return ((this->*(this->replies[ERR_NICKNAMEINUSE]))(formatedMessage, &this->sendClients[prefix]));
	if (remoteUser.getStatus() == USER)
		(this->*(this->replies[RPL_NICKBROADCAST]))(formatedMessage, &remoteUser);
	setNick(&remoteUser, formatedMessage, true, this->sendClients, this->clientList);
	return (CONNECT);
}


int					Server::remoteNickHandler(const Message &message, Client *client)
{
	std::string	prefix;

	if (message.getPrefix() == "")
		return (this->setRemoteNick(message, client));
	prefix = message.getPrefix().substr(1, message.getPrefix().length());
	if (this->sendClients.count(prefix) && !this->clientList.count(prefix)
	&& !this->serverList.count(prefix))
		return (this->resetRemoteNick(message, client));
	return ((this->*(this->replies[ERR_PREFIX]))(message, client));
}

int					Server::nickHandler(const Message &message, Client *client)
{
	client->setCurrentCommand("NICK");
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
	std::string		realName;
	std::string		userName;

	userName = (message.getParameter(0)[0] == '~' ?
	message.getParameter(0).substr(1, message.getParameter(0).length())
	: message.getParameter(0));
	realName = (message.getParameter(3)[0] == ':' ?
	message.getParameter(3).substr(1, message.getParameter(3).length())
	: message.getParameter(3));
	client->setInfo(USERNAME, userName);
	client->setInfo(UPLINKSERVER, serverName);
	client->setInfo(ADDRESS, address);
	client->setInfo(REALNAME, realName);
	if (client->getInfo(NICK) != "")
	{
		(sendClients[client->getInfo(NICK)]).setInfo(USERNAME, userName);
		(sendClients[client->getInfo(NICK)]).setInfo(UPLINKSERVER, serverName);
		(sendClients[client->getInfo(NICK)]).setInfo(ADDRESS, address);
		(sendClients[client->getInfo(NICK)]).setInfo(REALNAME, realName);
	}
}

int					Server::setLocalUser(const Message &message, Client *client)
{
	if (client->getStatus() == UNKNOWN)
	{
		if (message.getParameters().size() != 4)
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
		if (!message.getPrefix().empty()
		&& message.getPrefix() != std::string(":") + client->getInfo(NICK))
			return ((this->*(this->replies[ERR_PREFIX]))(message, client));
		return ((this->*(this->replies[ERR_ALREADYREGISTRED]))(message, client));
	}
	return (CONNECT);
}

static bool			isVaildIpAddress(const Message &message)
{
	for(size_t i = 0; i < message.getParameter(1).length(); i++)
	{
		if (i == 0 && !isValidFormat(std::string(DIGIT), message.getParameter(1)[i]))
			return (false);
		if (i == message.getParameter(1).length() - 1 && isValidFormat(std::string(".:"), message.getParameter(1)[i]))
			return (false);
		else if (!isValidFormat(std::string(DIGIT) + std::string(".:"), message.getParameter(1)[i]))
			return (false);
	}
	return (true);
}

int					Server::setRemoteUser(const Message &message, Client *client)
{
	std::string							prefix;
	std::list<std::string>::iterator	iterator;
	std::list<std::string>				serverList;

	if (message.getPrefix().empty())
		return ((this->*(this->replies[ERR_PREFIX]))(message, client));
	prefix = message.getPrefix().substr(1, message.getPrefix().length());
	if (!this->sendClients.count(prefix) || this->clientList.count(prefix)
	|| this->serverList.count(prefix))
		return ((this->*(this->replies[ERR_PREFIX]))(message, client));
	if (this->sendClients[prefix].getStatus() == USER)
		return ((this->*(this->replies[ERR_ALREADYREGISTRED]))(message, client));
	if (this->sendClients[prefix].getStatus() != UNKNOWN)
		return ((this->*(this->replies[ERR_PREFIX]))(message, client));
	if (message.getParameters().size() != 4)
		return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
	if (!isVaildIpAddress(message))
		return (CONNECT);
	this->getChildServer(serverList, client->getInfo(SERVERNAME));
	serverList.push_back(client->getInfo(SERVERNAME));
	iterator = std::find(serverList.begin(), serverList.end(), message.getParameter(2));
	if (iterator == serverList.end() || this->sendClients[*iterator].getStatus() != SERVER)
		return (CONNECT);
	setUser(message, &this->sendClients[prefix], message.getParameter(1), this->sendClients, message.getParameter(2));
	return (this->*(this->replies[RPL_REGISTERUSER]))(message, &this->sendClients[prefix]);
}

int					Server::userHandler(const Message &message, Client *client)
{
	client->setCurrentCommand("USER");
	if (client->getStatus() == SERVER)
		return (this->setRemoteUser(message, client));
	return (this->setLocalUser(message, client));
}
