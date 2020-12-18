#include "Server.hpp"

int					Server::passHandler(const Message &message, Client *client)
{
	if (message.getParameters().empty())
		this->sendMessage(Message(this->prefix, ERR_NEEDMOREPARAMS, " :Not enough parameters"), client);
	else if (client->getIsAuthorized())
		this->sendMessage(Message(this->prefix, ERR_ALREADYREGISTRED, " :You already reregistered"), client);
	else if (this->pass == message.getParameter(0))
	{
		// TODO privmsg함수로 바꿔야함
		send(client->getFd(), "Password accepted\r\n", 20, 0);
		client->setIsAuthorized(true);
	}
	else
		this->sendMessage(Message("", ERROR_STR, ": You put a wrong Password"), client);
	return (0);
}