#include "Server.hpp"

int		Server::eNoNickNameGivenHandler(const Message &message, Client *client)
{
	std::string	parameters;

	(void)message;
	parameters = std::string(":No nickname given");
	Message sendMessage(this->prefix, ERR_NONICKNAMEGIVEN, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::eNeedMoreParamsHandler(const Message &message, Client *client)
{
	std::string parameters;
	Message		sendMessage;

	parameters = client->getInfo(NICK) == "" ? "*" : client->getInfo(NICK);
	parameters += std::string(" ");
	parameters += message.getCommand();
	parameters += std::string(" :Syntax error");
	sendMessage = Message(this->prefix, ERR_NEEDMOREPARAMS, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::eErroneusNickNameHandler(const Message &message, Client *client)
{
	std::string	parameters;
	Message		sendMessage;

	parameters = message.getParameter(0);
	if (client->getStatus() == SERVER)
	{
		parameters += std::string(" ");
		parameters += client->getInfo(SERVERNAME);
	}
	parameters += std::string(" :Erroneous nickname");
	sendMessage = Message(this->prefix, ERR_ERRONEUSNICKNAME, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::eNickNameInUseHandler(const Message &message, Client *client)
{
	std::string	parameters;
	Message		sendMessage;

	parameters = client->getInfo(NICK) == "" ? "*" : client->getInfo(NICK);
	parameters += std::string(" ");
	parameters += message.getParameter(0);
	parameters += std::string(" :Nickname already in use");
	sendMessage = Message(this->prefix, ERR_NICKNAMEINUSE, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::eAlreadyRegisteredHandler(const Message &message, Client *client)
{
	std::string	parameters;
	Message		sendMessage;
	int			connectionStatus;

	connectionStatus = CONNECT;
	if (message.getCommand() != "USER")
	{
		parameters = std::string(":ID ");
		parameters += message.getParameter(0);
		parameters += std::string(" already registered");
		connectionStatus = DISCONNECT;
	}
	else if (client->getStatus() == USER)
	{
		parameters = client->getInfo(NICK);
		parameters += std::string(" :Connection already registered");
	}
	else
		parameters = std::string(":Unauthorized command (already registered)");
	sendMessage = Message(this->prefix, ERR_ALREADYREGISTRED, parameters);
	this->sendMessage(sendMessage, client);
	return (connectionStatus);
}

int		Server::ePassUnauthorizedHandler(const Message &message, Client *client)
{
	std::string	parameters;
	Message		sendMessage;

	(void)message;
	parameters += std::string(":Access denied: Bad password");
	sendMessage = Message("", ERROR_STR, parameters);
	this->sendMessage(sendMessage, client);
	return (DISCONNECT);
}

int		Server::eErroneusUserNameHandler(const Message &message, Client *client)
{
	std::string	parameters;
	Message		sendMessage;

	parameters = message.getCommand();
	parameters += std::string(" :Erroneous username");
	sendMessage = Message("", ERROR_STR, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::ePrefixHandler(const Message &message, Client *client)
{
	std::string	parameters;
	Message		sendMessage;

	parameters = std::string(":Invalid prefix \"");
	if (!message.getPrefix().empty())
		parameters += message.getPrefix()[0] == ':'
		? message.getPrefix().substr(1, message.getPrefix().length())
		: message.getPrefix();
	parameters += std::string("\"");
	sendMessage = Message(std::string(""), ERROR_STR, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::eNickCollisionHandler(const Message &message, Client *client)
{
	std::string		prefix;
	std::string		parameters;
	Message			sendMessage;

	prefix = this->prefix;
	parameters = message.getParameter(0);
	parameters += std::string(" :Nick collision");
	sendMessage = Message(prefix, ERR_NICKCOLLISION, parameters);
	this->sendMessage(sendMessage, client);
	this->disconnectClient(message, client);
	return (CONNECT);
}

int		Server::eCantKillServerHandler(const Message &message, Client *client)
{
	std::string		prefix;
	std::string		parameters;
	Message			sendMessage;

	prefix = this->prefix;
	parameters = message.getParameter(0);
	parameters += std::string(" :You can't kill a server!");
	sendMessage = Message(prefix, ERR_CANTKILLSERVER, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::eNoSuchServer(const Message &message, Client *client)
{
	std::string		prefix;
	std::string		parameters;
	Message			sendMessage;

	prefix = this->prefix;
	if (message.getPrefix().empty())
		parameters = client->getInfo(NICK);
	else
		parameters = message.getPrefix().substr(1, message.getPrefix().length());
	parameters += " ";
	parameters += message.getParameter(0);
	parameters += " :No such server";
	sendMessage = Message(prefix, ERR_NOSUCHSERVER, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::eUnknownCommand(const Message &message, Client *client)
{
	std::string		prefix;
	std::string		parameters;
	Message			sendMessage;

	prefix = this->prefix;
	if (message.getPrefix().empty())
		parameters = client->getInfo(NICK);
	else
		parameters = message.getPrefix().substr(1, message.getPrefix().length());
	parameters += " ";
	if (!message.getParameters().empty())
		parameters += message.getParameter(0);
	parameters += " :Unknown command";
	sendMessage = Message(prefix, ERR_UNKNOWNCOMMAND, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}


int		Server::eNotRegistered(const Message &message, Client *client)
{
	(void)message;
	sendMessage(Message(this->prefix
						, ERR_NOTREGISTERED
						, "* :Connection not registered")
						, client);
	return (CONNECT);
}

int		Server::eNoRecipients(const Message &message, Client *client)
{
	std::string		prefix;
	std::string		parameters;
	Message			sendMessage;

	(void)message;
	prefix = this->prefix;
	parameters += client->getInfo(NICK);
	parameters += std::string(" :No recipient given (privmsg)");
	sendMessage = Message(prefix, ERR_NORECIPIENT, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::eNoTextToSend(const Message &message, Client *client)
{
	std::string		prefix;
	std::string		parameters;
	Message			sendMessage;

	(void)message;
	prefix = this->prefix;
	parameters += client->getInfo(NICK);
	parameters += std::string(" :No text to send");
	sendMessage = Message(prefix, ERR_NOTEXTTOSEND, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::eNoSuchNick(const Message &message, Client *client)
{
	std::string		prefix;
	std::string		parameters;
	Message			sendMessage;

	prefix = this->prefix;
	parameters += client->getInfo(NICK);
	parameters += std::string(" ");
	parameters += message.getParameter(0);
	parameters += std::string(" :No such nick or channel name");
	sendMessage = Message(prefix, ERR_NOSUCHNICK, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int     Server::eNoOrigin(const Message &message, Client *client)
{
    (void)message;
    sendMessage(Message(this->prefix
                        , ERR_NOORIGIN
                        , client->getInfo(NICK)
                        + " :No Origin Specified")
                        , client);
    return (CONNECT);
}