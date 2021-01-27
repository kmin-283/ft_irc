#include "Server.hpp"

int					Server::passHandler(const Message &message, Client *client)
{
	client->setCurrentCommand("PASS");
	if (message.getParameters().empty())
		this->sendMessage(Message(this->prefix, ERR_NEEDMOREPARAMS, " :Not enough parameters"), client);
	else if (client->getIsAuthorized())
		this->sendMessage(Message(this->prefix, ERR_ALREADYREGISTRED, " :You already reregistered"), client);
	else if (this->pass == message.getParameter(0))
		client->setIsAuthorized(true);
	if (client->getStatus() == USER)
		this->infosPerCommand[client->getCurrentCommand()].incrementLocalCount(1);
	else if (client->getStatus() == SERVER)
		this->infosPerCommand[client->getCurrentCommand()].incrementRemoteCount(1);
	return (CONNECT);
}
