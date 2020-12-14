#include "Server.hpp"
void				Server::passHandler(Message &message, Client &client)
{
	/*
	std::cout << "pass handler" << std::endl;
	std::string returnMessage;

	returnMessage += this->prefix;
	if (0 < message.getParameters().length())
	{
		returnMessage += std::string(" ");
		returnMessage += std::string(ERR_NEEDMOREPARAMS);
		returnMessage += std::string(" :Not enough parameters");
		send(client->getFd(), returnMessage.c_str(), returnMessage.lenght(), 0);
	}
	else if (client->getIsAuthorized())
	{
		returnMessage += " ";
		returnMessage += ERR_NEEDMOREPARAMS;
		returnMessage += " :You may not reregister";
		send(client->getFd(), returnMessage.c_str(), returnMessage.lenght(), 0);

	}
	else if (this->pass == message.getParameter(0))
		client->setIsAuthorized(true);
		*/
	std::cout << "passHandler = "<< message.getParameter(0) << " " << client.getFd() << std::endl;
}

// void				Server::nickHandler(Message &message, Client &client)
// {

// }

// void				Server::userHandler(Message &message, Client &client)
// {
	

// }