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

void				Server::settingClient(const Message &message, Client *client)
{
	client->setStatus(SERVER);
	client->setInfo(SERVERNAME, message.getParameter(0));
	if (message.getParameters().size() == 4)
	{
		client->setInfo(TOKEN, message.getParameter(2));
		client->setInfo(SERVERINFO, message.getParameter(3));
	}
	else
	{
		client->setInfo(TOKEN, "0");
		client->setInfo(SERVERINFO, message.getParameter(2));
	}
}

int					Server::serverHandler(const Message &message, Client *client)
{
	if (client->getStatus() == UNKNOWN)
	{
		if (!client->getIsAuthorized() || (3 > message.getParameters().size()) // nc로 입력할 때 토큰 없이 입력하는 경우 3
		|| (message.getParameter(0).find('.') == std::string::npos)
		|| client->getInfo(NICK) != "" || client->getInfo(USERNAME) != "")
		{
			return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
		}
		if (this->sendClients.count(message.getParameter(0))
		|| this->serverName == message.getParameter((0)))
			return ((this->*(this->replies[ERR_ALREADYREGISTRED]))(message, client));
		settingClient(message, client);
		client->setInfo(UPLINKSERVER, this->serverName);
		client->setInfo(HOPCOUNT, std::string("1"));
		this->sendClients[message.getParameter(0)] = *client;
		this->serverList[message.getParameter(0)] = client;
		return ((this->*(this->replies[RPL_SERVER]))(message, client));
	}
	else if (client->getStatus() == SERVER)
	{
		if (message.getPrefix() == ""
		|| !this->sendClients.count(message.getPrefix().substr(1, message.getPrefix().length()))) // 서버연결시에 새로운 연결일 수도 있음
			return (CONNECT);
		if (message.getParameters().size() <= 3) // 첫 연결시에 :localhost.3000 SERVER localhost.3000 1 : kmin seunkim dakim made this server. ==> parameter가 4가 아님
			return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
		if (this->serverName == message.getParameter((0)) ||
		(this->sendClients.count(message.getParameter(0))))
		{
			// (this->*(this->replies[RPL_SQUITBROADCAST]))(message, client);
			return ((this->*(this->replies[ERR_ALREADYREGISTRED]))(message, client));
		}
		if (message.getParameter(1) == "1") // 홉카운트가 1
		{
			settingClient(message, client);
			client->setInfo(UPLINKSERVER, this->serverName);
			client->setInfo(HOPCOUNT, std::string("1"));
			this->sendClients[message.getParameter(0)] = *client;
			this->serverList[message.getParameter(0)] = client;
		}
		else
		{
			Client newClient(client->getFd());

			settingClient(message, &newClient);
			newClient.setInfo(UPLINKSERVER, message.getPrefix().substr(1, message.getPrefix().length()));
			newClient.setInfo(HOPCOUNT, message.getParameter(1));
			this->sendClients[message.getParameter(0)] = newClient;
		}
		(this->*(this->replies[RPL_SERVERBROADCAST]))(message, client);
	}
	else if (client->getStatus() == USER)
	{
		// syntax error
	}
	return (CONNECT);
}

/*
 * 인자가 2개가 아닌 경우 syntax error
 * 없는 servername(localhost.locahost) 이거나 잘못된 servername(asdfasdf)인 경우 모두 무시
 * 
 * 
 * 
 * 
 * 
 *
 * 
 * 나와 1대1로 연결된 서버인 경우 WALLOPS와 SQUIT을 보내면 됨 (홉카운트 1)
 *
 * 하지만 나와 1대1로 연결되지 않은 remote서버인 경우 해당 remote서버에게만 메시지를 보냄
 * 연결을 종료하는 것이 아닌 리스트에서 지우기만 하면 됨.
 * 그런데 이때 SQUIT명령어 그대로를 보내야 할 듯
 * Sa --- Sb --- Sc // Sc가 SQUIT Sa :badlink이면 Sb에는 메시지가 가지 않고 Sa에게만 메시지를 보냄
 * 
 * 
 * 
 * 
 * Sa -- Sb -- Sc   // Sa가 SQUIT Sc :badlink 이면 먼저 Sd를 SQUIT 그 다음 Sc SQUIT
 *              |
 * 			   Sd
 * 
 * 
 * nc 명령어를 쓰면 결국 ngircd에게 명령을 보내는 것이라는 점을 기억하자!
 */

int					Server::squitHandler(const Message &message, Client *client)
{
	std::map<std::string, Client*>::iterator	it;

	if (message.getParameters().size() != 2)
	{
		std::cout << "111" << std::endl;
		return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
	}

	// 유저모드가 w이면 메시지를 보내야 한다고 함 <-- 추가해야할 내용...?


	if (message.getParameter(0) == this->serverName)
	{
		std::map<std::string, Client*>::iterator	it;
		this->run = false;
		// 현재 서버가 uplink인 자식 서버들?에게 연결이 끊어지라고 메시지를 보내야 함 SQUIT serverName :bad
		for (it = this->serverList.begin(); it != this->serverList.end(); ++it)
			sendMessage(Message("", "SQUIT", it->second->getInfo(SERVERNAME) + " " + message.getParameter(1)), it->second);
		std::cout << "ERROR: \"" << message.getParameter(1) << "\" (SQUIT from " << message.getPrefix() << ")" << std::endl;
		return (TOTALDISCONNECT);
	}
	for (it = this->serverList.begin(); it != this->serverList.end(); ++it)
	{
		std::cout << "222" << std::endl;
		if (it->second->getInfo(SERVERNAME) == message.getParameter(0)) // 직접적으로 연결된 서버
		{
			// sendMessage(Message("WALLOPS\r\n"), it->second); // 다시 모든 serverList를 돌면서 WALLOPS메시지를 보냄
			// 추가로 SQUIT 메시지도 보냄

			std::cout << "2.5 2.5 2.5" << std::endl;
			sendMessage(message, it->second);
			disconnectClient(it->second);
			return (CONNECT);
		}
	}
	// 직접적으로 연결되지 않은 클라이언트 ---> operator 이거나 다른 server이거나
	// if (client->getStatus() == SERVER)
	// {
		std::cout << "333" << std::endl;
		std::string tmp;

		if (message.getPrefix() == "")
		{
			tmp = ":";
			tmp += this->serverName;
			tmp += " ";
			tmp += message.getTotalMessage();
			broadcastMessage(Message(tmp), client);
		}
		else
			broadcastMessage(message, client);
		this->sendClients.erase(message.getParameter(0));
	// }
	// else
	// {
	// 	std::cout << "444" << std::endl;
	// }
	return (CONNECT);
}