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

Client		*Server::hasTarget(const std::string &target, strClientPtrIter start, strClientPtrIter end)
{
	for (; start != end; ++start)
	{
		if (target == start->second->getInfo(SERVERNAME))
			return (start->second);
	}
	return (NULL);
}

void		Server::incrementLcountAndByte(const std::string &command, const Message &message)
{
	this->infos[command].incrementLocalCount(1);
	this->infos[command].incrementBytes(message.getTotalMessage().length());
}

void		Server::incrementRcountAndByte(const std::string &command, const Message &message)
{
	this->infos[command].incrementRemoteCount(1);
	this->infos[command].incrementBytes(message.getTotalMessage().length());
}

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

	ret->reserve(10);
	for (it = this->sendClients.begin(); it != this->sendClients.end(); ++it)
	{
		if (it->second.getStatus() == SERVER && match(const_cast<char *>(info.c_str()), const_cast<char *>(it->second.getInfo(SERVERNAME).c_str())))
			ret->push_back(it->second.getInfo(SERVERNAME));
	}
	if (match(const_cast<char *>(info.c_str()), const_cast<char *>(this->serverName.c_str())))
		ret->push_back(this->serverName);
	if (ret->empty())
	{
		delete ret;
		return (NULL);
	}
	return (ret);
}

/*
 * version 앞에 인자가 없는 경우 에러
 * version 뒤에 인자가 하나도 없어도 에러
 * version 앞에 정상적인 인자 뒤에는 인자가 없어도 출력 가능
 */

int Server::versionHandler(const Message &message, Client *client)
{
	std::vector<std::string>	*list;
	std::string					check;

	client->setCurrentCommand("VERSION");
	if ((check = client->prefixCheck(message)) != "ok")
		return (this->*(this->replies[check]))(message, client);
	if (!message.getParameters().empty())
		list = getInfoFromWildcard(message.getParameter(0));
	else
		list = getInfoFromWildcard(this->serverName);
	if (client->getStatus() == USER)
	{
		// user에게선 prefix가 없는 경우만 옴
		if (!list || *(--list->end()) == this->serverName)
		{
			sendMessage(Message(this->prefix, RPL_VERSION, client->getInfo(NICK) + " " + this->version + ". " + this->serverName), client);
			if (list && !list->empty())
				list->pop_back();
		}
		else if (list->empty())
			(this->*(this->replies[ERR_NOSUCHSERVER]))(message, client);
		// remote server에 대한 version 요청
		else if (list)
		{
			for (std::vector<std::string>::iterator it = list->begin(); it != list->end(); ++it)
				sendMessage(Message(":" + client->getInfo(NICK), "VERSION", *it)
							, &this->sendClients[*it]);
		// 다른 서버에 version 요청
		}
	}
	else if (client->getStatus() == SERVER)
	{
		// server에게선 prefix가 있는 경우만 처리하면 됨
		if (message.getCommand() == RPL_VERSION)
		{
			Client *ret;

			if (message.getParameter(0) != this->serverName)
			{
				ret = hasTarget(message.getParameter(0), this->serverList.begin(), this->serverList.end());
				if (ret == NULL)
					ret = hasTarget(message.getParameter(0), this->clientList.begin(), this->clientList.end());
				if (ret != NULL)
					sendMessage(message, ret);
				else
					sendMessage(message, &this->sendClients[message.getParameter(0)]);
			}
			delete list;
			return (CONNECT);
		}
		if (list->empty())
			(this->*(this->replies[ERR_NOSUCHSERVER]))(message, client);
		else if (!list || *(--list->end()) == this->serverName)
		{
			sendMessage(Message(this->prefix
						, RPL_VERSION
						, message.getPrefix().substr(1, message.getPrefix().length()) + " " + this->version + ". " + this->serverName)
						, client);
			if (list && !list->empty())
				list->pop_back();
		}
		for (std::vector<std::string>::iterator it = list->begin(); it != list->end(); ++it)
			sendMessage(Message(message.getPrefix(), "VERSION", *it)
						, &this->sendClients[*it]);
	}
	delete list;
	return (CONNECT);
}

/*
 * STATS m 지원되는 명령어 사용횟수, 사용되는 바이트 크기를 리턴함
 * :from STATS 212 to Command lcount(나에게 온 요청) bytes rcount(remote server에서 온 요청)
 *
 *
 */

// VERSION, STATS에서 와일드 카드 문제 해결하기
// 현재 와일드 카드 변환이 사용된 위치보다 먼저 와일드카드 문제를 해결해야함

// stats 은 무조건 1개의 서버를 대상으로 함
int			Server::statsHandler(const Message &message, Client *client)
{
	std::vector<std::string>	*list;
	std::string					check;

	client->setCurrentCommand("STATS");
	if ((check = client->prefixCheck(message)) != "ok")
		return (this->*(this->replies[check]))(message, client);
	if (message.getParameters().size() == 2)
		list = getInfoFromWildcard(message.getParameter(1));
	else
		list = NULL;
	if (client->getStatus() == USER)
	{
		if (message.getParameters().size() == 0)
			(this->*(this->replies[RPL_ENDOFSTATS]))(message, client);
		else if (!list || *(--list->end()) == this->serverName)
		{
			(this->*(this->replies[std::string("STATS_") + message.getParameter(0)[0]]))(message, client);
			(this->*(this->replies[RPL_ENDOFSTATS]))(message, client);
			if (list)
				list->clear();
		}
		else if (list->empty())
			(this->*(this->replies[ERR_NOSUCHSERVER]))(message, client);
		else if (!list->empty())
			sendMessage(Message(":" + client->getInfo(NICK), "VERSION", *(--list->end()))
						, &this->sendClients[*(--list->end())]);
		// 다른 서버에 version 요청
	}
	else if (client->getStatus() == SERVER)
	{
		if (message.getCommand() == "STATS")
		{
			if (message.getParameters().empty())
				(this->*(this->replies[RPL_ENDOFSTATS]))(message, client);
			else if (!this->replies.count(std::string("STATS_") + message.getParameter(0)[0]))
				(this->*(this->replies[RPL_ENDOFSTATS]))(message, client);
			else if (!list || *(--list->end()) == this->serverName)
			{
				(this->*(this->replies[std::string("STATS_") + message.getParameter(0)[0]]))(message, client);
				(this->*(this->replies[RPL_ENDOFSTATS]))(message, client);
			}
			else if (list->empty())
				return (this->*(this->replies[ERR_NOSUCHSERVER]))(message, client);
			else
				sendMessage(Message(message.getPrefix()
									, message.getCommand()
									, message.getParameter(0)[0]
									+ std::string(" ") + *(--list->end()))
									, &this->sendClients[*(--list->end())]);
		}
		else
		{
			Client *ret;

			if (message.getParameter(0) != this->serverName)
			{
				ret = hasTarget(message.getParameter(0), this->serverList.begin(), this->serverList.end());
				if (ret == NULL)
					ret = hasTarget(message.getParameter(0), this->clientList.begin(), this->clientList.end());
				if (ret != NULL)
					sendMessage(message, ret);
				else
					sendMessage(message, &this->sendClients[message.getParameter(0)]);
			}
		}
	}
	delete list;
	return (CONNECT);
}

int			Server::linksHandler(const Message &message, Client *client)
{
	std::string					check;
	std::vector<std::string>	*list;

	client->setCurrentCommand("LINKS");
	if ((check = client->prefixCheck(message)) != "ok")
		return (this->*(this->replies[check]))(message, client);
	if (!message.getParameters().empty())
		list = getInfoFromWildcard(message.getParameter(0));
	else
		list = getInfoFromWildcard("*");
	if (!list)
	{
		delete list;
		return (this->*(this->replies[ERR_NOSUCHSERVER]))(message, client);
	}
	std::string parameter;

	for (std::vector<std::string>::iterator it = list->begin(); it != list->end(); ++it)
	{
		if (message.getPrefix().empty())
			parameter = client->getInfo(NICK);
		else
			parameter = message.getPrefix().substr(1, message.getPrefix().length());
		if (*it == this->serverName)
		{
				sendMessage(Message(this->prefix
									, RPL_LINKS
									, parameter
									+ " " + this->serverName
									+ " " + this->serverName
									+ " :0 " + this->info)
									, client);
		}
		else
		{
			sendMessage(Message(this->prefix
							, RPL_LINKS
							, parameter
							+ " " + this->sendClients[*it].getInfo(SERVERNAME)
							+ " " + this->sendClients[*it].getInfo(UPLINKSERVER)
							+ " :" + this->sendClients[*it].getInfo(HOPCOUNT)
							+ " " + this->sendClients[*it].getInfo(SERVERINFO))
							, client);
		}
	}
	(this->*(this->replies[RPL_ENDOFLINKS]))(message, client);
	delete list;
	return (CONNECT);
}

int			Server::timeHandler(const Message &message, Client *client)
{
	std::string					check;
	std::vector<std::string>	*list;
	time_t						now;

	client->setCurrentCommand("TIME");
	if ((check = client->prefixCheck(message)) != "ok")
		return (this->*(this->replies[check]))(message, client);
	now = time(NULL);
	if (message.getParameters().size() == 1)
		list = getInfoFromWildcard(message.getParameter(0));
	else if (message.getParameters().size() > 1)
		list = new std::vector<std::string>;
	else
		list = getInfoFromWildcard(this->serverName);
	if (!list)
		(this->*(this->replies[ERR_NOSUCHSERVER]))(message, client);
	else if (client->getStatus() == USER)
	{
		if (message.getParameters().empty() || *(--list->end()) == this->serverName)
		{
			sendMessage(Message(this->prefix
								, RPL_TIME
								, this->serverName
								+ " :" + getTimestamp(now, false))
								, client);
		}
		else
		{
			sendMessage(Message(":" + client->getInfo(NICK)
								, "TIME"
								, message.getParameter(0))
								, &this->sendClients[message.getParameter(0)]);
		}
	}
	else if (client->getStatus() == SERVER)
	{
		if (message.getCommand() == "TIME")
		{
			if (message.getParameters().empty() || message.getParameter(0) == this->serverName)
			{
				sendMessage(Message(this->prefix
									, RPL_TIME
									, message.getPrefix().substr(1, message.getPrefix().length())
									+ " " + this->serverName
									+ " :" + getTimestamp(now, false))
									, client);
			}
			else
				broadcastMessage(message, &this->sendClients[message.getParameter(0)]);
		}
		else
		{
			Client *ret;

			ret = hasTarget(message.getParameter(0), this->serverList.begin(), this->serverList.end());
			if (ret == NULL)
				ret = hasTarget(message.getParameter(0), this->clientList.begin(), this->clientList.end());
			if (ret != NULL)
				sendMessage(message, ret);
			else
				broadcastMessage(message, &this->sendClients[message.getParameter(0)]);
		}
	}
	delete list;
	return (CONNECT);
}

/*
 * connect 명령어를 위해선 ngircd conf파일에 있는 host에 127.0.0.1, port에 해당 포트 3000, 3001등을 입력해두어야 한다.
 * 그 후 ngircd에 접속하여 OPER oper oper 명령어, oper아이디, oper비밀번호를 입력하여 관리자 권한으로 connect명령어를 수행할 수 있다.
 *
 * 예를들어 3001번 포트에 자체제작한 서버를 실행시키고 관리자가 ngircd에 connect localhost.3001을 하면
 *
 * 스스로 돌아가고 있는 3001번 서버에 입력이 들어온다.
 */

int				Server::connectHandler(const Message &message, Client *client)
{
	std::string check;

	client->setCurrentCommand("CONNECT");
	if ((check = client->prefixCheck(message)) != "ok")
		return (this->*(this->replies[check]))(message, client);
	if (message.getParameters().empty() || message.getParameters().size() == 2)
		return (this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client);
	if (!isValidAddress(message.getParameter(0)))
		return (this->*(this->replies[ERR_NOSUCHSERVER]))(message, client);
	if (client->getStatus() == USER)
	{
		//if (client->getMode() != OPER)
			//return noprivileges
		if (message.getParameters().size() == 1)
		{
			this->connectServer(message.getParameter(0));
			// wallops message
		}
		else
		{
			sendMessage(Message(":" + client->getInfo(NICK)
								, "CONNECT"
								, message.getParameter(0)
								+ " " + message.getParameter(1)
								+ " " + message.getParameter(2))
								, &this->sendClients[message.getParameter(2)]);
		}
	}
	else if (client->getStatus() == SERVER)
	{
		if (message.getParameter(2) == this->serverName)
		{
			this->connectServer(message.getParameter(0));
			// wallops message
		}
		else
			sendMessage(message, &this->sendClients[message.getParameter(2)]);
	}
	return (CONNECT);
}
