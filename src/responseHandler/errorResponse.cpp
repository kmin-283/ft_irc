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

	(void)message;
	parameters += std::string(":Unauthorized command (already registered)");
	sendMessage = Message(this->prefix, ERR_ALREADYREGISTRED, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
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