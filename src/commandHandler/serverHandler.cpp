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

void Server::settingClient(const Message &message, Client *client)
{
    size_t parameterSize;

    parameterSize = message.getParameters().size();
	client->setStatus(SERVER);
	client->setInfo(SERVERNAME, message.getParameter(0));
	if (parameterSize == 4)
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

int Server::serverHandler(const Message &message, Client *client)
{
	client->setCurrentCommand("SERVER");
	if (client->getStatus() == UNKNOWN)
	{
		this->infosPerCommand[client->getCurrentCommand()].incrementLocalCount(1);
		if (!client->getIsAuthorized() || (message.getParameters().size() < 3) // nc로 입력할 때 토큰 없이 입력하는 경우 3
			|| (message.getParameter(0).find('.') == std::string::npos) || client->getInfo(NICK) != "" || client->getInfo(USERNAME) != "")
		{
			return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
		}
		if (this->sendClients.count(message.getParameter(0)) || this->serverName == message.getParameter((0)))
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
		this->infosPerCommand[client->getCurrentCommand()].incrementRemoteCount(1);
		if (message.getPrefix() == "")// 서버연결시에 새로운 연결일 수도 있음
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
 *           3000
 *          /  |  
 *      3001  3003
 *     /
 *  3002
 */

void Server::deleteSubServers(const std::string &targetServer, const std::string &info) {
    std::map<std::string, Client>::iterator it;
    std::map<std::string, Client>::iterator prev;
    bool                                    goToBegin;

    it = this->sendClients.begin();
    prev = this->sendClients.begin();
    goToBegin = false;
    while (it != this->sendClients.end())
    {
        if (it->second.getStatus() == USER)
        {
            if (it->second.getInfo(HOSTNAME) == targetServer)
            {
                goToBegin = true;
                this->sendClients.erase(it->first);
            }
        }
        else
        {
            if (it->second.getInfo(UPLINKSERVER) == targetServer)
            {
                goToBegin = true;
                deleteSubServers(it->second.getInfo(SERVERNAME), info);
            }
        }
        if (goToBegin)
        {
            it = this->sendClients.begin();
            goToBegin = false;
        }
        else
            ++it;
    }
    this->sendClients.erase(targetServer);
}

int Server::squitHandler(const Message &message, Client *client)
{
	std::map<std::string, Client *>::iterator   it;
	std::string                                 from;

	client->setCurrentCommand("SQUIT");
	//if (message.getPrefix().empty() && (client->getInfo(MODE) != OPERATOR
	// || message.getParameter(0) == this->serverName))
	  //  return NO_PRIVILIGED
	if (message.getParameters().size() != 2)
		return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
    // 유저모드가 w이면 메시지를 보내야 한다고 함 <-- 추가해야할 내용...?
    if (message.getPrefix().empty())
        from = client->getInfo(NICK);
    else
        from = message.getPrefix().substr(1, message.getPrefix().length());
	if (message.getParameter(0) == this->serverName) // 현재 서버가 삭제되어야 할 서버인 경우
	{
		this->run = false;
		broadcastMessage(message, client);
		return (TOTALDISCONNECT);
	}
	else if (this->serverList.count(message.getParameter(0)))
    {
	    for (strClientPtrIter it = this->serverList.begin(); it != this->serverList.end(); ++it)
        {
            if (client->getInfo(SERVERNAME) != message.getParameter(0))
            {
                sendMessage(Message(this->prefix
                                    , "WALLOPS"
                                    , "Received SQUIT "
                                    + message.getParameter(0)
                                    + " from " + from
                                    + " " + message.getParameter(1))
                                    , it->second);
            }
            if (it->second->getInfo(SERVERNAME) != client->getInfo(SERVERNAME))
            {
                sendMessage(Message(":" + from
                                    , "SQUIT"
                                    , message.getParameter(0)
                                    + " " + message.getParameter(1))
                                    , it->second);
            }
        }
        return (DISCONNECT);
    }
    else if (this->sendClients.count(message.getParameter(0)))
    {
        // 재귀적으로 돌면서 연결된 모든 서버를 지워줘야 함. //
	    deleteSubServers(message.getParameter(0), message.getParameter(1));
	    broadcastMessage(Message(":" + from
                                , "SQUIT"
                                , message.getParameter(0)
                                + " " + message.getParameter(1))
                                , client);
    }
    else
        return ((this->*(this->replies[ERR_NOSUCHSERVER]))(message, client));
    return (CONNECT);
}

int     Server::pingHandler(const Message &message, Client *client)
{
    std::string					check;
    std::string					from;
    std::string                 target;
    std::vector<std::string>	*list;
    size_t						parameterSize;

    client->setCurrentCommand("PING");
    if (client->getStatus() == UNKNOWN)
        return (this->*(this->replies[ERR_NOTREGISTERED]))(message, client);
    else if (client->getStatus() == USER)
        this->infosPerCommand[client->getCurrentCommand()].incrementLocalCount(1);
    else
        this->infosPerCommand[client->getCurrentCommand()].incrementRemoteCount(1);
    if ((check = client->prefixCheck(message)) != "ok")
        return (this->*(this->replies[check]))(message, client);

    parameterSize = message.getParameters().size();
    if (parameterSize > 2)
        return (this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client);
    else if (parameterSize == 0)
        return (this->*(this->replies[ERR_NOORIGIN]))(message, client);
    else if (parameterSize == 1)
    {
        if (message.getParameter(0)[0] == ':')
        {
            target = message.getParameter(0).substr(1, message.getParameter(0).length());
            list = getInfoFromWildcard(target);
        }
        else
        {
            sendMessage(Message(this->prefix
                                , "PONG"
                                , this->serverName
                                + " :" + message.getParameter(0))
                                , client);
            return (CONNECT);
        }
    }
    else
    {
        if (message.getParameter(1)[0] == ':')
        {
            target = message.getParameter(1).substr(1, message.getParameter(1).length());
            list = getInfoFromWildcard(target);
        }
        else
        {
            target = message.getParameter(1);
            list = getInfoFromWildcard(target);
        }
    }
    if (!list)
    {
        delete list;
        return (this->*(this->replies[ERR_NOSUCHSERVER]))(message, client);
    }
    if (message.getPrefix().empty())
        from = client->getInfo(NICK);
    else
        from = message.getPrefix().substr(1, message.getPrefix().length());
    if (parameterSize == 1 || *(--list->end()) == this->serverName)
    {
        sendMessage(Message(this->prefix
                            , "PONG"
                            , this->serverName + " " + from)
                            , &this->sendClients[from]);
    }
    else
    {
        sendMessage(Message(':' + from
                            , "PING"
                            , message.getParameter(0) +
                            " :" + target)
                            , &this->sendClients[target]);
    }
    return (CONNECT);
}

/*
 * Sa(ngircd) --- Sb(3000) --- Sc(3002)
 *                |
 *                Ua(asd)
 * 인 경우 user는 갱신안되는 문제 해결하기
 */


int     Server::pongHandler(const Message &message, Client *client)
{
    std::string     target;
    size_t          parameterSize;

    client->setCurrentCommand("PONG");
    parameterSize = message.getParameters().size();
    if (parameterSize == 0)
        return (this->*(this->replies[ERR_NOORIGIN]))(message, client);
    if (parameterSize != 1)
    {
        if (message.getParameter(1)[0] == ':')
            target = message.getParameter(1).substr(1, message.getParameter(1).length());
        else
            target = message.getParameter(1);
    }
    if (message.getParameters().size() == 1 || target == this->serverName)
    {
        client->setWaitPong(true);
        client->setLastPing(std::time(NULL));
        client->setPingLimit(std::time(NULL));
    }
    else if (this->sendClients.count(target))
        sendMessage(message, &this->sendClients[target]);
    return (CONNECT);
}