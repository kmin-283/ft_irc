#include "Server.hpp"

int         Server::wallopsHandler(const Message&message, Client *client)
{
	if (message.getParameters().size() < 1)
		return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
	if (client->getStatus() == USER)
	{

	}
	else // client == SERVER
	{
		std::cout << message.getTotalMessage();
	}
	return (CONNECT);
}

// :irc.example.net WALLOPS 
// :Received SQUIT localhost.6671 from localhost.6670 :bad


//서버 연결을 끊을 때 출력되는 메시지를 어떻게 처리할 것인가