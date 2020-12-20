#include "Server.hpp"

int		Server::rRegisterUserHandler(const Message &message, Client *client)
{
	client->setStatus(USER);
	this->sendClients[message.getParameter(0)].setStatus(USER);
	(this->*(this->replies[RPL_NICKBROADCAST]))(message, client);
	return (CONNECT);
}

int		Server::rWelcomeMessageHandler(const Message &message, Client *client)
{
	(this->*(this->replies[RPL_WELCOME]))(message, client);
	(this->*(this->replies[RPL_YOURHOST]))(message, client);
	(this->*(this->replies[RPL_CREATED]))(message, client);
	(this->*(this->replies[RPL_MYINFO]))(message, client);
	(this->*(this->replies[RPL_LUSERCLIENT]))(message, client);
	(this->*(this->replies[RPL_LUSERCHANNELS]))(message, client);
	(this->*(this->replies[RPL_LUSERME]))(message, client);
	(this->*(this->replies[RPL_MOTD]))(message, client);
	return (CONNECT);
}

int		Server::rWelcomeHandler(const Message &message, Client *client)
{
	std::string		parameters;
	Message			sendMessage;

	(void)message;
	parameters = client->getInfo(NICK);
	parameters += std::string(" :Welcome to the Internet Relay Network ");
	parameters += client->getInfo(NICK);
	parameters += std::string("!~");
	parameters += client->getInfo(USERNAME);
	parameters += std::string("@");
	parameters += client->getInfo(HOSTNAME);
	sendMessage = Message(this->prefix, RPL_WELCOME, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::rYourHostHandler(const Message &message, Client *client)
{
	std::string		parameters;
	Message			sendMessage;

	(void)message;
	parameters = client->getInfo(NICK);
	parameters += std::string(" :Your host is ");
	parameters += this->serverName;
	parameters += std::string(", running version ");
	parameters += this->version;
	sendMessage = Message(this->prefix, RPL_YOURHOST, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::rCreatedHandler(const Message &message, Client *client)
{
	std::string		parameters;
	Message			sendMessage;

	(void)message;
	parameters = client->getInfo(NICK);
	parameters += std::string(" :This server has been started ");
	parameters += this->startTime;
	sendMessage = Message(this->prefix, RPL_CREATED, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::rMyInfoHandler(const Message &message, Client *client)
{
	std::string		parameters;
	Message			sendMessage;

	(void)message;
	parameters = client->getInfo(NICK);
	parameters += std::string(" ");
	parameters += this->serverName;
	parameters += std::string(" ");
	parameters += this->version;
	parameters += std::string(" ");
	parameters += this->userMode;
	parameters += std::string(" ");
	parameters += this->channelMode;
	sendMessage = Message(this->prefix, RPL_MYINFO, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

static void		countServerConnections(std::map<std::string, Client> &sendClients,
							int &userCount, int &serviceCount, int &serverCount)
{
	std::map<std::string, Client>::iterator	iterator;

	userCount = 0;
	serviceCount = 0;
	serverCount = 0;
	for(iterator = sendClients.begin(); iterator != sendClients.end(); ++iterator)
	{
		if (iterator->second.getStatus() == SERVER)
			++serverCount;
		else if (iterator->second.getStatus() == USER)
			++userCount;
		else if (iterator->second.getStatus() == SERVICE)
			++serviceCount;
	}
}

int		Server::rLUserClientHandler(const Message &message, Client *client)
{
	std::stringstream	stream;
	std::string			parameters;
	Message				sendMessage;
	int					userCount;
	int					serviceCount;
	int					serverCount;

	(void)message;
	countServerConnections(this->sendClients, userCount, serviceCount, serverCount);
	parameters = client->getInfo(NICK);
	parameters += std::string(" :There are ");
	stream << userCount;
	parameters += stream.str();
	stream.str("");
	parameters += std::string(" users and ");
	stream << serviceCount;
	parameters += stream.str();
	stream.str("");
	parameters += std::string(" services on ");
	stream << ++serviceCount;
	parameters += stream.str();
	parameters += std::string(" servers");
	sendMessage = Message(this->prefix, RPL_LUSERCLIENT, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::rLUserChannelHandler(const Message &message, Client *client)
{
	std::stringstream	stream;
	std::string			parameters;
	Message				sendMessage;

	(void)message;
	parameters = client->getInfo(NICK);
	parameters += std::string(" ");
	stream << this->channelList.size();
	parameters += stream.str();
	parameters += std::string(" :channels formed");
	sendMessage = Message(this->prefix, RPL_LUSERCHANNELS, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::rLUserMeHandler(const Message &message, Client *client)
{
	std::stringstream	stream;
	std::string			parameters;
	Message				sendMessage;

	(void)message;
	parameters = client->getInfo(NICK);
	parameters += std::string(" :I have ");
	stream << this->clientList.size();
	parameters += stream.str();
	stream.str("");
	parameters += std::string(" users, ");
	stream << this->serviceList.size();
	parameters += stream.str();
	stream.str("");
	parameters += std::string(" services and ");
	stream << this->serviceList.size();
	parameters += stream.str();
	stream.str("");
	parameters += std::string(" servers");
	sendMessage = Message(this->prefix, RPL_LUSERME, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::rMOTDHandler(const Message &message, Client *client)
{
	(this->*(this->replies[RPL_MOTDSTART]))(message, client);
	(this->*(this->replies[RPL_MOTDCONTENT]))(message, client);
	(this->*(this->replies[RPL_ENDOFMOTD]))(message, client);
	return (CONNECT);
}

int		Server::rMOTDStartHandler(const Message &message, Client *client)
{
	std::string	parameters;
	Message		sendMessage;

	(void)message;
	parameters = client->getInfo(NICK);
	parameters += std::string(" :- ");
	parameters += this->serverName;
	parameters += std::string(" message of the day");
	sendMessage = Message(this->prefix, RPL_MOTDSTART, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::rEndOfMOTDHandler(const Message &message, Client *client)
{
	std::string	parameters;
	Message		sendMessage;

	(void)message;
	parameters = client->getInfo(NICK);
	parameters += std::string(" :End of MOTD command");
	sendMessage = Message(this->prefix, RPL_ENDOFMOTD, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::rMOTDContentHandler(const Message &message, Client *client)
{
	std::ifstream	stream;
	std::string		prefix;
	std::string		readResult;
	std::string		parameters;
	Message			sendMessage;

	(void)message;
	prefix = client->getInfo(NICK);
	prefix += std::string(" :- ");
	stream.open(this->motdDir);
	if (!stream)
	{
		std::cerr << ERROR_FILE_OPEN_FAIL << std::endl;
		parameters = prefix;
		parameters += std::string("42cursus c5 ft_irc made by kmin seunkim dakim");
		sendMessage = Message(this->prefix, RPL_MOTDCONTENT, parameters);
		this->sendMessage(sendMessage, client);
		return (CONNECT);
	}
	while (stream)
	{
		getline(stream, readResult);
		if (readResult != "")
		{
			parameters = prefix;
			parameters += readResult.substr(0, 79);
			sendMessage = Message(this->prefix, RPL_MOTDCONTENT, parameters);
			this->sendMessage(sendMessage, client);
		}
	}
	stream.close();
	return (CONNECT);
}

int		Server::rNickBroadcastHandler(const Message &message, Client *client)
{
	std::string			parameters;
	Message				sendMessage;

	(void)message;
	parameters = client->getInfo(NICK);
	parameters += std::string(" :");
	parameters += client->getInfo(HOPCOUNT);
	sendMessage = Message(std::string(""), RPL_NICK, parameters);
	this->broadcastMessage(sendMessage, client);
	return (CONNECT);
}
