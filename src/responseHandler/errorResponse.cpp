#include "Server.hpp"

int		Server::eNoNickNameGivenHandler(const Message &message, Client *client)
{
	std::string	parameters;

	(void)message;
	parameters = ":No nickname given";
	Message sendMessage(this->prefix, ERR_NONICKNAMEGIVEN, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::eNeedMoreParamsHandler(const Message &message, Client *client)
{
	std::string parameters;
	Message		sendMessage;

	(void)message;
	parameters = client->getInfo(CURRENTNICK) == "" ? "*" : client->getInfo(CURRENTNICK);
	parameters += std::string(" NICK :Syntax error");
	sendMessage = Message(this->prefix, ERR_NEEDMOREPARAMS, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::eErroneusNickNameHandler(const Message &message, Client *client)
{
	std::string	parameters;
	Message		sendMessage;

	parameters = message.getParameter(0);
	parameters += " :Erroneous nickname";
	sendMessage = Message(this->prefix, ERR_ERRONEUSNICKNAME, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::eNickNameInUseHandler(const Message &message, Client *client)
{
	std::string	parameters;
	Message		sendMessage;

	parameters = client->getInfo(CURRENTNICK) == "" ? "*" : client->getInfo(CURRENTNICK);
	parameters += " ";
	parameters += message.getParameter(0);
	parameters += " :Nickname already in use";
	sendMessage = Message(this->prefix, ERR_NICKNAMEINUSE, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}
