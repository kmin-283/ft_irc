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

	(void)message;
	parameters = client->getInfo(CURRENTNICK) == "" ? "*" : client->getInfo(CURRENTNICK);
	parameters += std::string(" NICK :Syntax error");
	Message sendMessage(this->prefix, ERR_NEEDMOREPARAMS, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::eErroneusNickNameHandler(const Message &message, Client *client)
{
	std::string	parameters;

	parameters = message.getParameter(0);
	parameters += " :Erroneous nickname";
	Message sendMessage(this->prefix, ERR_ERRONEUSNICKNAME, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}

int		Server::eNickNameInUseHandler(const Message &message, Client *client)
{
	std::string	parameters;

	parameters = client->getInfo(CURRENTNICK) == "" ? "*" : client->getInfo(CURRENTNICK);
	parameters += " ";
	parameters += message.getParameter(0);
	parameters += " :Nickname already in use";
	Message sendMessage(this->prefix, ERR_NICKNAMEINUSE, parameters);
	this->sendMessage(sendMessage, client);
	return (CONNECT);
}
