#include "Server.hpp"

int					Server::passHandler(const Message &message, Client *client)
{
	client->setCurrentCommand("PASS");
	if (message.getParameters().empty())
		this->sendMessage(Message(this->prefix, ERR_NEEDMOREPARAMS, " :Not enough parameters"), client);
	else if (client->getIsAuthorized())
		this->sendMessage(Message(this->prefix, ERR_ALREADYREGISTRED, " :You already reregistered"), client);
	else if (this->pass == message.getParameter(0))
	{
		this->sendMessage(Message("", "Password accepted", ""), client);
		client->setIsAuthorized(true);
	}
	return (CONNECT);
}
