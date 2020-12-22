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

void			Server::sendAllInfo(Client *client)
{
	std::map<std::string, Client>::iterator it;

	this->sendMessage(Message(this->prefix, "SERVER",
			this->serverName + " "
			+ "1" + " "
			+ this->info), client);

	for(it = this->sendClients.begin(); it != this->sendClients.end(); ++it)
	{
		if (it->second.getInfo(SERVERNAME) != client->getInfo(SERVERNAME))
		{
			this->sendMessage(Message(":" + it->second.getInfo(UPLINKSERVER), "SERVER",
			it->second.getInfo(SERVERNAME) + " "
			+ std::to_string(ft_atoi(it->second.getInfo(HOPCOUNT).c_str()) + 1) + " "
			+ it->second.getInfo(SERVERINFO)), client);
		}
	}
}


int					Server::serverHandler(const Message &message, Client *client)
{
	std::cout << "in" << std::endl;
	if (client->getStatus() == UNKNOWN)
	{
		std::cout << "1" << std::endl;
		if (!client->getIsAuthorized() || (3 != message.getParameters().size())
		|| (message.getParameter(0).find('.') == std::string::npos))
			return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
		if (this->sendClients.find(message.getParameter(0)) != this->sendClients.end()
		|| this->serverName == message.getParameter((0)))
		{
			std::cout << message.getParameter(0) << std::endl;
			return ((this->*(this->replies[ERR_ALREADYREGISTRED]))(message, client));
		}
		client->setStatus(SERVER);
		client->setInfo(UPLINKSERVER, this->serverName);
		client->setInfo(SERVERNAME, message.getParameter(0));
		client->setInfo(HOPCOUNT, std::string("1"));
		client->setInfo(SERVERINFO, message.getParameter(2));
		this->sendClients[message.getParameter(0)] = *client;
		this->serverList[message.getParameter(0)] = client;
		return ((this->*(this->replies[RPL_SERVER]))(message, client));
	}
	else if (client->getStatus() == SERVER)
	{
		std::cout << "2" << std::endl;
		if (message.getPrefix() == ""
		|| !this->sendClients.count(message.getPrefix().substr(1, message.getPrefix().length())))
			return (CONNECT);
		if (message.getParameters().size() != 4)
			return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
		if (this->sendClients.count(message.getParameter(0)))
			return ((this->*(this->replies[ERR_ALREADYREGISTRED]))(message, client));
		if (this->sendClients.find(message.getParameter(0)) != this->sendClients.end()
		|| this->serverName == message.getParameter((0)))
		{
			// (this->*(this->replies[RPL_SQUITBROADCAST]))(message, client);	
			return ((this->*(this->replies[ERR_ALREADYREGISTRED]))(message, client));	
		}
	}
	else if (client->getStatus() == USER)
	{
		// syntax error
	}

	// if (!client->getIsAuthorized() || (3 > message.getParameters().size())
	// || (message.getParameter(0).find('.') == std::string::npos))
	// // || (message.getPrefix() != client->getPrefix()))
	// {
	// 	return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
	// }
	// if ((this->sendClients.find(message.getParameter(0)) != this->sendClients.end())
	// || (message.getPrefix() == "" &&
	// this->prefix.substr(1, this->prefix.length()) == message.getParameter((0))))
	// {
	// 	this->sendMessage(Message("", "ERROR", ":ID " + message.getParameter(0) + " already registered"), client);
	// 	return (DISCONNECT);
	// }
	// if (message.getParameter(1) != "1") // hopcount가 1이 아닌 경우
	// {
	// 	Client newClient(client->getFd());
	// 	// client->getStatus() == SERVER;
	// 	message.getPrefix() == "" ? newClient.setInfo(UPLINKSERVER, this->prefix.substr(1, this->prefix.length()))
	// 	: newClient.setInfo(UPLINKSERVER, message.getPrefix().substr(1, message.getPrefix().length()));
	// 	newClient.setInfo(SERVERNAME, message.getParameter(0));
	// 	message.getPrefix() == "" ? newClient.setInfo(HOPCOUNT, "1")
	// 	: newClient.setInfo(HOPCOUNT, message.getParameter(1));
	// 	newClient.setInfo(SERVERINFO, message.getParameter(2));
	// 	this->sendClients[message.getParameter(0)] = newClient;
	// 	// this->serverList[message.getParameter(0)] = &this->sendClients[message.getParameter(0)];
	// }
	// else // hopcount가 1인 경우
	// {
	// 	message.getPrefix() == "" ? client->setInfo(UPLINKSERVER, this->prefix.substr(1, this->prefix.length()))
	// 	: client->setInfo(UPLINKSERVER, message.getPrefix().substr(1, message.getPrefix().length()));
	// 	client->setInfo(SERVERNAME, message.getParameter(0));
	// 	client->setInfo(HOPCOUNT, message.getParameter(1));
	// 	client->setInfo(SERVERINFO, message.getParameter(2));

	// 	this->sendClients[message.getParameter(0)] = *client;
	// 	this->serverList[message.getParameter(0)] = client;
	// }
	// if (message.getPrefix() == "")
	// {
	// 	if (client->getStatus() == UNKNOWN)
	// 	{
	// 		client->setStatus(SERVER);
	// 		sendMessage(Message(this->prefix, "PASS", this->pass), client);
	// 		sendAllInfo(client);
	// 	}
	// 	broadcastMessage(Message(this->prefix, message.getCommand(), client->getInfo(SERVERNAME) +
	// 	" " + std::to_string(ft_atoi(message.getParameter(1).c_str()) + 1) +
	// 	" " + message.getParameter(2)), client);
	// 	return (CONNECT);
	// }
	// broadcastMessage(Message(message.getPrefix(), message.getCommand(), message.getParameter(0) +
	// " " + std::to_string(ft_atoi(message.getParameter(1).c_str()) + 1) +
	// " " + message.getParameter(2)), client);
	// std::cout << message.getTotalMessage();
	return (CONNECT);
}
