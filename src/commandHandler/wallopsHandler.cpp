#include "Server.hpp"
/*
 * wallops는 서버로부터 악용되는 사례때문에 서버로부터 발송되는게 바람직 하다고 함
 * 따라서 서버로부터 온 메시지만 처리하자.
 */
int         Server::wallopsHandler(const Message&message, Client *client)
{
	std::string	check;

	client->setCurrentCommand("WALLOPS");
	if ((check = client->prefixCheck(message)) != "ok")
		return (this->*(this->replies[check]))(message, client);
	this->infos[client->getCurrentCommand()].incrementRemoteCount(1);
	// for (strClientPtrIter it = this->clientList.begin(); it != this->clientList.end(); ++it)
	// {
		// if (it->second->getInfo()) client의 모드가 operator이면 wallops를 해당 operator에게 전달해줌.
	// }
	broadcastMessage(message, client);
	return (CONNECT);
}

// :irc.example.net WALLOPS 
// :Received SQUIT localhost.6671 from localhost.6670 :bad


//서버 연결을 끊을 때 출력되는 메시지를 어떻게 처리할 것인가