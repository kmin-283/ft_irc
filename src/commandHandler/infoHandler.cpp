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

void		Server::incrementLocalByte(Client *client, const Message &message)
{
    this->infos[client->getCurrentCommand()].incrementBytes(message.getTotalMessage().length());
}

void		Server::incrementRemoteByte(Client *client, const Message &message)
{
    this->infos[client->getCurrentCommand()].incrementBytes(message.getTotalMessage().length());
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
    std::string					toClient;
    size_t						parameterSize;

    client->setCurrentCommand("VERSION");
    if (client->getStatus() == UNKNOWN)
        return (this->*(this->replies[ERR_NOTREGISTERED]))(message, client);
    else if (client->getStatus() == USER)
        this->infos[client->getCurrentCommand()].incrementLocalCount(1);
    else
        this->infos[client->getCurrentCommand()].incrementRemoteCount(1);

    if ((check = client->prefixCheck(message)) != "ok")
        return (this->*(this->replies[check]))(message, client);

    parameterSize = message.getParameters().size();
    if (parameterSize > 1)
        return (this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client);
    else if (parameterSize == 1)
        list = getInfoFromWildcard(message.getParameter(0));
    else
        list = getInfoFromWildcard(this->serverName);
    if (!list)
    {
        delete list;
        return (this->*(this->replies[ERR_NOSUCHSERVER]))(message, client);
    }

    if (message.getPrefix().empty())
        toClient = client->getInfo(NICK);
    else
        toClient = message.getPrefix().substr(1, message.getPrefix().length());
    if (*(--list->end()) == this->serverName)
    {
        sendMessage(Message(this->prefix
                , RPL_VERSION
                , toClient
                  + " " + this->version
                  + ". " + this->serverName)
                , client);
        list->pop_back();
    }
    else
    {
        sendMessage(Message(":" + toClient
                , "VERSION"
                , *(--list->end()))
                , &this->sendClients[*(--list->end())]);
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
    std::string					from;
    size_t						parameterSize;
    std::map<std::string, int (Server::*)(const Message &, Client *)>::iterator	query;

    client->setCurrentCommand("STATS");
    if (client->getStatus() == UNKNOWN)
        return (this->*(this->replies[ERR_NOTREGISTERED]))(message, client);
    else if (client->getStatus() == USER)
        this->infos[client->getCurrentCommand()].incrementLocalCount(1);
    else
        this->infos[client->getCurrentCommand()].incrementRemoteCount(1);

    if ((check = client->prefixCheck(message)) != "ok")
        return (this->*(this->replies[check]))(message, client);


    parameterSize = message.getParameters().size();
    if (parameterSize == 0)
        return (this->*(this->replies[RPL_ENDOFSTATS]))(message, client);
    else if ((query = this->replies.find(std::string("STATS_") + message.getParameter(0)[0])) == this->replies.end())
        return (this->*(this->replies[RPL_ENDOFSTATS]))(message, client);
    else if (parameterSize > 2)
        return (this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client);
    else if (parameterSize == 2)
        list = getInfoFromWildcard(message.getParameter(1));
    else
        list = getInfoFromWildcard(this->serverName);

    if (!list)
    {
        delete list;
        return (this->*(this->replies[ERR_NOSUCHSERVER]))(message, client);
    }

    if (*(--list->end()) == this->serverName)
    {
        (this->*(query->second))(message, client);
        //(this->*(this->replies[std::string("STATS_") + message.getParameter(0)[0]]))(message, client);
        (this->*(this->replies[RPL_ENDOFSTATS]))(message, client);
    }
    else
    {
        if (message.getPrefix().empty())
            from = ":" + client->getInfo(NICK);
        else
            from = message.getPrefix();
        sendMessage(Message(from
                , "STATS"
                , message.getParameter(0)
                  + " " + *(--list->end()))
                , &this->sendClients[*(--list->end())]);
    }
    delete list;
    return (CONNECT);
}

int			Server::linksHandler(const Message &message, Client *client)
{
    std::string					check;
    std::string					parameter;
    std::vector<std::string>	*list;
    size_t						parameterSize;

    client->setCurrentCommand("LINKS");
    if (client->getStatus() == UNKNOWN)
        return (this->*(this->replies[ERR_NOTREGISTERED]))(message, client);
    else if (client->getStatus() == USER)
        this->infos[client->getCurrentCommand()].incrementLocalCount(1);
    else
        this->infos[client->getCurrentCommand()].incrementRemoteCount(1);
    if ((check = client->prefixCheck(message)) != "ok")
        return (this->*(this->replies[check]))(message, client);

    parameterSize = message.getParameters().size();
    if (parameterSize > 2)
        return (this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client);
    else if (parameterSize == 2)
        list = getInfoFromWildcard(message.getParameter(1));
    else if (parameterSize == 1)
        list = getInfoFromWildcard(message.getParameter(0));
    else
        list = getInfoFromWildcard("*");
    if (!list)
    {
        delete list;
        return (this->*(this->replies[ERR_NOSUCHSERVER]))(message, client);
    }

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
    std::string					toClient;
    std::vector<std::string>	*list;
    time_t						now;
    size_t						parameterSize;

    client->setCurrentCommand("TIME");
    if (client->getStatus() == UNKNOWN)
        return (this->*(this->replies[ERR_NOTREGISTERED]))(message, client);
    else if (client->getStatus() == USER)
        this->infos[client->getCurrentCommand()].incrementLocalCount(1);
    else
        this->infos[client->getCurrentCommand()].incrementRemoteCount(1);
    if ((check = client->prefixCheck(message)) != "ok")
        return (this->*(this->replies[check]))(message, client);

    now = time(NULL);
    parameterSize = message.getParameters().size();
    if (parameterSize > 1)
        return (this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client);
    else if (parameterSize == 1)
        list = getInfoFromWildcard(message.getParameter(0));
    else
        list = getInfoFromWildcard(this->serverName);
    if (!list)
    {
        delete list;
        (this->*(this->replies[ERR_NOSUCHSERVER]))(message, client);
    }

    if (message.getPrefix().empty())
        toClient = client->getInfo(NICK);
    else
        toClient = message.getPrefix().substr(1, message.getPrefix().length());
    if (*(--list->end()) == this->serverName)
    {
        sendMessage(Message(this->prefix
                , RPL_TIME
                , toClient
                  + " :" + getTimestamp(now, false))
                , client);
    }
    else
    {
        sendMessage(Message(":" + toClient
                , "TIME"
                , *(--list->end()))
                , &this->sendClients[*(--list->end())]);
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
    std::string from;
    size_t		parameterSize;

    client->setCurrentCommand("CONNECT");
    if (client->getStatus() == UNKNOWN)
        return (this->*(this->replies[ERR_NOTREGISTERED]))(message, client);
    else if (client->getStatus() == USER)
        this->infos[client->getCurrentCommand()].incrementLocalCount(1);
    else
        this->infos[client->getCurrentCommand()].incrementRemoteCount(1);
    if ((check = client->prefixCheck(message)) != "ok")
        return (this->*(this->replies[check]))(message, client);

    parameterSize = message.getParameters().size();
    if (parameterSize == 0 || parameterSize == 2)
        return (this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client);
    if (!isValidAddress(message.getParameter(0)))
        return (this->*(this->replies[ERR_NOSUCHSERVER]))(message, client);

    if (message.getPrefix().empty())
        from = client->getInfo(NICK);
    else
        from = message.getPrefix().substr(1, message.getPrefix().length());
    if (parameterSize == 1 || message.getParameter(2) == this->serverName)
    {
        broadcastMessage(Message(this->prefix
                , "WALLOPS"
                , ":Received CONNECT "
                  + message.getParameter(0)
                  + " from " + from)
                , client);
        if (message.getParameters().size() == 1)
            this->connectServer(message.getParameter(0));
    }
    else
    {
        sendMessage(Message(":" + from
                , "CONNECT"
                , message.getParameter(0)
                  + " " + message.getParameter(1)
                  + " " + message.getParameter(2))
                , &this->sendClients[message.getParameter(2)]);
    }
    return (CONNECT);
}

int				Server::traceHandler(const Message &message, Client *client)
{
    std::string					check;
    std::string					from;
    std::vector<std::string>	*list;
    size_t						parameterSize;
    strClientPtrIter			found;
    Client						*target;

    client->setCurrentCommand("TRACE");
    if (client->getStatus() == UNKNOWN)
        return (this->*(this->replies[ERR_NOTREGISTERED]))(message, client);
    else if (client->getStatus() == USER)
        this->infos[client->getCurrentCommand()].incrementLocalCount(1);
    else
        this->infos[client->getCurrentCommand()].incrementRemoteCount(1);
    if ((check = client->prefixCheck(message)) != "ok")
        return (this->*(this->replies[check]))(message, client);

    parameterSize = message.getParameters().size();
    if (parameterSize > 1)
        return (this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client);
    else if (parameterSize == 1)
        list = getInfoFromWildcard(message.getParameter(0));
    else
        list = getInfoFromWildcard(this->serverName);
    if (!list)
    {
        delete list;
        return (this->*(this->replies[ERR_NOSUCHSERVER]))(message, client);
    }
    if (message.getPrefix().empty())
        from = client->getInfo(NICK);
    else
        from = message.getPrefix().substr(1, message.getPrefix().length());
    found = this->clientList.find(*(--list->end()));
    target = &this->sendClients[from];
    if (*(--list->end()) == this->serverName || found != this->clientList.end())
    {
        //if (client->getInfo(MODE) == OPERATOR)
        //{
        //RPL_TRACEOPER
        //RPL_TRACEUSER
        //처리하기
        //}
        for (strClientPtrIter it = this->serverList.begin(); it != this->serverList.end(); ++it)
        {
            sendMessage(Message(this->prefix
                    , RPL_TRACESERVER
                    , from + " Serv 1 0S 0C "
                      + it->second->getInfo(SERVERNAME)
                      + " *|*@" + this->serverName
                      + " :V")
                    , target);
        }
        sendMessage(Message(this->prefix
                , RPL_TRACEEND
                , from + " " + this->serverName
                  + " " + this->version + " :End of TRACE")
                , target);
    }
    else
    {
        sendMessage(Message(this->prefix
                , RPL_TRACELINK
                , from + " Link "
                  + this->version + " " + *(--list->end())
                  + " " + this->serverName + " V"
                  + " " + getTimestamp(std::time(NULL) - this->startTime, true)
                  + " 0 0")
                , target);
        sendMessage(Message(":" + from
                , "TRACE"
                , *(--list->end()))
                , &this->sendClients[*(--list->end())]);
    }
    return (CONNECT);
}
