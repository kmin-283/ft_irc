#include "Server.hpp"
/**
 * 1. PASS 인증 한 경우
 * 	1-1. 인자가 제대로 들어온 경우
 * 		1-1-1. 서버 명 중복
 * 		=> ERROR :ID "localhost.6670" already registered
 * 		=> 연결 해제
 * 		1-1-2. 서버 명 중복 x
 * 			1-1-2-1. 직접 연결된 서버인 경우
 * 			=> :irc.example.net SERVER irc.example.net 1 :Server Info Text
 * 			1-1-2-2. 직접 연결되지 않은 서버인 경우
 * 			=> :irc.example.net SERVER localhost.6671 2 3 :123123
 * 	1-2. 인자가 제대로 들어오지 않은 경우
 * 	=> 461에러 리턴
 * 2. PASS 인증 하지 않은 경우 v
 * => 461
 * => 함수 종료
 **/
int					Server::serverHandler(const Message &message, Client *client)
{
	if (!client->getIsAuthorized() || (3 > message.getParameters().size())
	|| (message.getParameter(0).find('.') == std::string::npos))
	// || (message.getPrefix() != client->getPrefix()))
	{
		this->sendNumericReplies(Message(this->prefix, ERR_NEEDMOREPARAMS, "* SERVER :Syntax error"), client);
		return (ERROR);
	}
	if ((this->sendClients.find(message.getParameter(0)) != this->sendClients.end())
	|| (this->prefix.substr(1, this->prefix.length()) == message.getParameter((0))))
	{
		std::cout << "server haneler2" << std::endl;
		this->sendNumericReplies(Message("", "ERROR", " :ID " + message.getParameter(0) + " already registered"), client);
		this->disconnectClient(client);
		return (ERROR);
	}
	client->setInfo(message);
	// client->Info[REMOTESERVERS] = ;
	this->sendClients[message.getParameter(0)] = *client;
	return (0);
	// 연결된 다른 서버에 서버 알리기
}
