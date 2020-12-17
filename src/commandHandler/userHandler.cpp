#include "Server.hpp"

int					Server::userHandler(const Message &message, Client *client)
{
	(void)message;
	(void)client;
	// std::string		userNick;

	// userNick = client->getCurrentNick() == "" ? "*" : client->getCurrentNick();
	// if (message.getParameters().size() != 4)
		// this->sendNumericReplies(Message(this->prefix, ERR_NEEDMOREPARAMS, std::string(" ") + userNick + std::string(" USER :Syntax error")), client);
	// else if ()
		// this->sendNumericReplies(ERR_ALREADYREGISTRED, std::string(" ") + userNick + std::string(" :Connection already registered"), client);
	// else if (!client->getIsAuthorized())
	// 	this->sendNumericReplies(Message(this->prefix, ERROR_STR, std::string(" ") + userNick + std::string(":Bad password")), client);
	// else
	// {
	// 	client->registerUser(message.getParameters());
	// 	if (client->isClientRegistered())
	// 		client->setStatus(USER);
	// 	if (client->getStatus() == USER)
	// 		std::cout << "client status = client" << std::endl;
	// }
	return (0);
}

