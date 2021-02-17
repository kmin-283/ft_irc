#include "Server.hpp"

static bool			isValidNickName(const Message &message)
{
	for (size_t i = 0; i < message.getParameter(0).length(); i++)
	{
		if (i == 0 && !isInTheMask(std::string(LETTER) + std::string(SPECIAL), message.getParameter(0)[i]))
			return false;
		else if (!isInTheMask(std::string(LETTER) + std::string(SPECIAL) + std::string(DIGIT), message.getParameter(0)[i]))
			return false;
	}
	return true;
}

static bool			isVaildUserName(const Message &message)
{
	for (size_t i = 0; i < message.getParameter(0).length(); i++)
	{
		if (isInTheMask(std::string(USER_FORMAT), message.getParameter(0)[i]))
			return false;
	}
	return true;
}

static bool			isValidUserMode(const std::string &string)
{
	for (size_t i = 0; i < string.length(); i++)
	{
		if (i != 0 && !isInTheMask(std::string(MODEUSER), string[i]))
			return (false);
		else if (!isInTheMask(std::string(MODEUSER) + std::string("+-"), string[i]))
			return (false);
	}
	return (true);
}

static bool			isValidHopCountToken(const std::string &string)
{
	for (size_t i = 0; i < string.length(); i++)
	{
		if (!isInTheMask(std::string(DIGIT), string[i]))
			return (false);
	}
	return (true);
}

static Message		setNickMessage(const Message &message)
{
	std::string			parameters;
	Message				returnMessage;

	parameters = message.getParameter(0).substr(1, message.getParameter(0).length());
	parameters += std::string(" :1");
	returnMessage = Message(std::string(""), RPL_NICK, parameters);
	return (returnMessage);
}

static Message		setUserMessage(const Message &message, std::string serverName)
{
	std::string		parameters;
	Message			returnMessage;

	parameters = message.getParameter(2);
	parameters += std::string(" ");
	parameters += message.getParameter(3);
	parameters += std::string(" ");
	parameters += serverName;
	parameters += std::string(" ");
	parameters += message.getParameter(6);
	returnMessage = Message(std::string(""), RPL_USER, parameters);
	return (returnMessage);
}

static bool			checkMessage(const Message &message)
{
	if (!isValidHopCountToken(message.getParameter(4)) || ft_atoi(message.getParameter(4).c_str()) != 1)
		return (false);
	if (message.getParameter(5)[0] == '-')
		return (false);
	if (!isValidUserMode(message.getParameter(5)))
		return (false);
	return (true);
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

static std::string	removeMode(const std::string &mode, const char key)
{
	std::string returnString;

	for (size_t i = 0; i < mode.length(); ++i)
	{
		if (mode[i] != key)
			returnString += mode[i];
	}
	return (returnString);
}

static void			setMode(const std::string &mode, Client *client,
					std::map<std::string, Client> &sendClients)
{
	size_t	i;
	bool	isSetMode;

	i = 0;
	isSetMode = true;
	if (mode[i] == '-')
		isSetMode = false;
	if (mode[i] == '+' || mode[i] == '-')
		++i;
	while (i < mode.length())
	{
		if (isSetMode && client->getInfo(USERMODE).find(mode[i]) == std::string::npos)
			client->setInfo(USERMODE, client->getInfo(USERMODE) + mode[i]);
		if (!isSetMode && client->getInfo(USERMODE).find(mode[i]) != std::string::npos)
			client->setInfo(USERMODE, removeMode(client->getInfo(USERMODE), mode[i]));
		++i;
	}
	sendClients[client->getInfo(NICK)].setInfo(USERMODE, client->getInfo(USERMODE));
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
	Message	userMessage;

	if (message.getParameters().size() != 2 && message.getParameters().size() != 7)
		return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
	if (!isValidNickName(message) || 9 < message.getParameter(0).length())
		return ((this->*(this->replies[ERR_ERRONEUSNICKNAME]))(message, client));
	if (this->sendClients.count(message.getParameter(0))
	|| this->serverName == message.getParameter(0))
		(this->*(this->replies[RPL_KILL]))(message, client);
	if (this->clientList.count(message.getParameter(0)))
		return ((this->*(this->replies[ERR_NICKCOLLISION]))(message, this->clientList[message.getParameter(0)]));
	if (this->sendClients.count(message.getParameter(0))
	&& this->sendClients[message.getParameter(0)].getStatus() == USER)
	{
		this->sendClients.erase(message.getParameter(0));
		return (CONNECT);
	}
	if ((this->sendClients.count(message.getParameter(0)) && this->sendClients[message.getParameter(0)].getStatus() == SERVER)
	|| this->serverName == message.getParameter(0))
		return ((this->*(this->replies[ERR_CANTKILLSERVER]))(message, client));
	if (message.getParameters().size() == 7 && !checkMessage(message))
		return (CONNECT);
	setNick(&remoteUser, message, true, this->sendClients, this->clientList);
	if (message.getParameters().size() == 7)
	{
		userMessage = setUserMessage(message, client->getInfo(SERVERNAME));
		setUser(userMessage, &remoteUser, userMessage.getParameter(1), this->sendClients, userMessage.getParameter(2));
		setMode(message.getParameter(5), &remoteUser, this->sendClients);
		return (this->*(this->replies[RPL_REGISTERUSER]))(message, &remoteUser);
	}
	return (CONNECT);
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
	formatedMessage = setNickMessage(message);
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
	if (this->getParentServer(prefix) == client->getInfo(NICK))
		return (this->setRemoteNick(message, client));
	if (this->sendClients.count(prefix) && !this->clientList.count(prefix)
	&& !this->serverList.count(prefix))
		return (this->resetRemoteNick(message, client));
	return ((this->*(this->replies[ERR_PREFIX]))(message, client));
}

int					Server::nickHandler(const Message &message, Client *client)
{
	client->setCurrentCommand("NICK");
	if (client->getStatus() == SERVER)
	{
		this->infosPerCommand[client->getCurrentCommand()].incrementRemoteCount(1);
		return (this->remoteNickHandler(message, client));
	}
	this->infosPerCommand[client->getCurrentCommand()].incrementLocalCount(1);
	return (this->localNickHandler(message, client));
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
	{
		this->infosPerCommand[client->getCurrentCommand()].incrementRemoteCount(1);
		return (this->setRemoteUser(message, client));
	}
	this->infosPerCommand[client->getCurrentCommand()].incrementLocalCount(1);
	return (this->setLocalUser(message, client));
}

int					Server::localQuitHandler(const Message &message, Client *client)
{
	if (client->getStatus() == UNKNOWN)
	{
		if (1 < message.getParameters().size())
			return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
		return ((this->*(this->replies[RPL_QUIT]))(message, client));
	}
	else if (client->getStatus() == USER)
	{
		if (message.getPrefix() != "")
			return ((this->*(replies[ERR_PREFIX]))(message, client));
		if (1 < message.getParameters().size())
			return ((this->*(replies[ERR_NEEDMOREPARAMS]))(message, client));
		(this->*(this->replies[RPL_QUITBROADCAST]))(message, client);
		return ((this->*(this->replies[RPL_QUIT]))(message, client));
	}
	return (CONNECT);
}

int					Server::remoteQuitHandler(const Message &message, Client *client)
{
	std::string prefix;

	if (message.getPrefix() == "" || 1 < message.getParameters().size())
		return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
	prefix = message.getPrefix()[0] == ':'
	? message.getPrefix().substr(1, message.getPrefix().length())
	: message.getPrefix();
	if (!this->sendClients.count(prefix)
	|| this->sendClients[prefix].getStatus() != USER
	|| this->getParentServer(prefix) != client->getInfo(SERVERNAME))
		return ((this->*(this->replies[ERR_PREFIX]))(message, client));
	this->sendClients.erase(prefix);
	return ((this->*(this->replies[RPL_QUITBROADCAST]))(message, client));
}

int					Server::quitHandler(const Message &message, Client *client)
{
	if (client->getStatus() == SERVER) {
		std::cout << "in quit " << message.getTotalMessage();
		Client *targetUser =  &this->sendClients[message.getPrefix().substr(1)];
		std::vector<std::string> *channelList = targetUser->getSubscribedChannelList();
		if (channelList != NULL) {
			for (size_t i = 0; i < channelList->size(); ++i) {
				if (this->localChannelList.count((*channelList)[i])) {
					this->localChannelList[(*channelList)[i]].leaveUser(targetUser);
				}
				if (this->remoteChannelList.count((*channelList)[i]))
					this->remoteChannelList[(*channelList)[i]].leaveUser(targetUser);
			}
		}
		delete channelList;
	}
	if (client->getStatus() == SERVER)
		return (this->remoteQuitHandler(message, client));
	return (this->localQuitHandler(message, client));
}
