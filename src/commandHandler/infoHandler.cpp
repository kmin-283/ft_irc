#include "Server.hpp"

/*
 * VERSION 명령어만 단독으로 쓰이는 경우에는 operator만 사용할 수 있는 것 같음
 * 
 * 반면에 VERSION <servername>을 사용하는 것은 아무 서버나 가능
 * 
 */

/*
 * wildcard는 ? * & 가 있다.
 * ?는 match a single character
 * * matches everything except
 * & matches a whole word if used alone
 * 
 * 
 * [example]
 *      *.se ---> .se로 끝나는 모든 서버
 * 
 */

static bool match(char *first, char *second)
{
	if (*first == 0 && *second == 0)
		return true;
	if (*first == '*' && *(first + 1) != '\0' && *second == '\0')
		return false;
	if (*first == '?' || *first == *second)
		return match(first + 1, second + 1);
	if (*first == '*')
		return match(first + 1, second) || match(first, second + 1);
	if (*first == '&' && (*(first + 1) == '.' || *(first + 1) == 0) && *second != '.' && *second != 0)
		return match(first, second + 1);
	if (*first == '&' && (*(first + 1) == '.' || *(first + 1) == 0) && (*second == '.' || *second == 0))
		return match(first + 1, second);
	return false;
}

std::vector<std::string> *Server::getInfoFromWildcard(const std::string &info)
{
	std::vector<std::string> *ret = new std::vector<std::string>;
	strClientIter it;

	ret->reserve(50);
	for (it = this->sendClients.begin(); it != this->sendClients.end(); ++it)
	{
		if (match(const_cast<char *>(info.c_str()), const_cast<char *>(it->second.getInfo(SERVERNAME).c_str())))
			ret->push_back(it->second.getInfo(SERVERNAME));
	}
	return ret;
}

int Server::versionHandler(const Message &message, Client *client)
{

	std::vector<std::string> *list;

	if (client->getStatus() == USER)
	{
		// user에게선 prefix가 없는 경우만 옴
		if (message.getParameters().size() == 0)
		{
			sendMessage(Message(this->prefix, RPL_VERSION, client->getInfo(NICK) + " " + this->version + ". " + this->serverName), client);
			return (CONNECT);
		}
		// remote server에 대한 version 요청
		list = getInfoFromWildcard(message.getParameter(0));
		for (std::vector<std::string>::iterator it = list->begin(); it != list->end(); ++it)
			broadcastMessage(Message(":" + client->getInfo(NICK), "VERSION", *it), client);
		// 다른 서버에 version 요청
		delete list;
	}
	else if (client->getStatus() == SERVER)
	{
		// server에게선 prefix가 있는 경우만 처리하면 됨
		if (message.getPrefix() != "")
		{
			if (message.getCommand() == RPL_VERSION)
			{
				if (message.getParameter(0) == this->serverName)
				{
					std::cout << message.getPrefix() << " " << message.getCommand() << " " << message.getTotalMessage();
					return (CONNECT);
				}
				broadcastMessage(message, client); // sendclient를 돌면서 서버와 유저에게 메시지를 보냄
				return (CONNECT);
			}
			list = getInfoFromWildcard(message.getParameter(0));
			if (match(const_cast<char *>(message.getParameter(0).c_str()), const_cast<char *>(this->serverName.c_str())))
			{    sendMessage(Message(this->prefix
							, RPL_VERSION
							, message.getPrefix().substr(1, message.getPrefix().length()) + " " + this->version + ". " + this->serverName)
							, client);
				if (list->size() == 0)
				{
					delete list;
					return (CONNECT);
				}
			}
			for (std::vector<std::string>::iterator it = list->begin(); it != list->end(); ++it)
				broadcastMessage(Message(message.getPrefix(), "VERSION", *it), client);
			delete list;
			return (CONNECT);
		}
	}
	// syntax error
	(this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client);
	return (CONNECT);
}

/*
 * STATS m 지원되는 명령어 사용횟수, 사용되는 바이트 크기를 리턴함
 * :from STATS 212 to Command lcount(나에게 온 요청) bytes rcount(나를 경유해간 요청)
 * 
 * 
 */

int			Server::statsHandler(const Message &message, Client *client)
{
	if (client->getStatus() == USER)
	{
		if (message.getParameters().size() > 0)
		{
			(this->*(this->replies["STATS_" + message.getParameter(0)]))(message, client);
			sendMessage(Message(this->prefix, RPL_VERSION, client->getInfo(NICK) + " " + this->version + ". " + this->serverName), client);
		}
		(this->*(this->replies[RPL_ENDOFSTATS]))(message, client);
	}
	else if (client->getStatus() == SERVER)
	{

	}
	(this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client);
	return (CONNECT);
}