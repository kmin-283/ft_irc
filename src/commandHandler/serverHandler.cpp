#include "Server.hpp"
/**
 * 1. PASS 인증 한 경우 v
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

// static void			broadcasting(const std::map<std::string, Client*> &serverList, const std::string &exceptName)
// {
// 	std::string sn("");

// 	for (std::map<std::string, Client *>::const_iterator it = serverList.begin(); it != serverList.end(); it++)
// 	{
// 		// std::cout << it->second->getInfo(SERVERNAME) << std::endl;
// 		if (it->first != exceptName)
// 		{
// 			sn += it->second->getInfo(UPLINKSERVER);
// 			sn += " SERVER";
// 			sn += " " + it->second->getInfo(SERVERNAME);
// 			sn += " " + std::to_string(ft_atoi(it->second->getInfo(HOPCOUNT).c_str()) + 1);
// 			sn += " " + it->second->getInfo(SERVERINFO);
// 			sn += "\r\n";
// 			send(it->second->getFd(), sn.c_str(), sn.length(), 0);
// 		}
// 	}
// }

int					Server::serverHandler(const Message &message, Client *client)
{
	// client->setInfo(SERVERNAME, "123123");
	// std::cout << "name - " << client->getInfo(SERVERNAME) << std::endl;
	// (void)message;
	client->setInfo(SERVERNAME, message.getParameter(0));
	client->setStatus(SERVER);
	this->serverList[message.getParameter(0)] = client;
	this->sendClients[message.getParameter(0)] = *client;
	this->broadcastMessage(Message("hello!\r\n"), client);

	// if (!client->getIsAuthorized()) // 얘를 어떻게 처리할 것인가
	// 	std::cout << "111\n";



	// if ((3 > message.getParameters().size()) // 잠시 || getIsAuthorized을 뺌
	// || (message.getParameter(0).find('.') == std::string::npos))
	// // || (message.getPrefix() != client->getPrefix()))
	// {
	// 	this->sendMessage(Message(this->prefix, ERR_NEEDMOREPARAMS, "* SERVER :Syntax error"), client);
	// 	return (ERROR);
	// }
	// if ((this->sendClients.find(message.getParameter(0)) != this->sendClients.end())
	// || (message.getPrefix() == "" &&
	// this->prefix.substr(1, this->prefix.length()) == message.getParameter((0))))
	// {
	// 	std::cout << "server haneler2" << std::endl;
	// 	this->sendMessage(Message("", "ERROR", " :ID " + message.getParameter(0) + " already registered"), client);
	// 	// this->disconnectClient(client);
	// 	return (DISCONNECT);
	// }
	// 클라이언트가 이미 존재하나, hopcount가 다른 경우 새로운 클라이언트를 생성해서 맵에 추가
	// client->setInfo(message, this->prefix);
	// this->sendClients[message.getParameter(0)] = *client;
	// if (client->getInfo(HOPCOUNT) == "1")
	// 	this->serverList[message.getParameter(0)] = client; // 추가함
	// // 연결된 다른 서버에 서버 알리기 --> 나와 1대1로 연결 된 서버(홉카운트 1) + uplink에게만 보내면 되나?
	// if (message.getPrefix() == "")
	// 	broadcasting(serverList, message.getParameter(0));

	return (0);
}
