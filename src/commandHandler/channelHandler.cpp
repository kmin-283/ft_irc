#include "Server.hpp"

std::vector<std::string>    getChannelNames(const std::string &name)
{
    std::vector<std::string>    names;
    std::stringstream           ss(name);
    std::string                 buffer;

    while (std::getline(ss, buffer, ','))
        names.push_back(buffer);
    return (names);
}

int     Server::joinHandler(const Message &message, Client *client)
{
    std::string                 check;
    std::string                 channelName;
    std::string                 fullChannelName;
    std::vector<std::string>    channelNames;
    std::vector<Client *>       joinedUsers;
    Channel                     *targetChannel;
    std::map<std::string, Channel>::iterator it;

    // 인자 애러 처리 해야함.

    client->setCurrentCommand("JOIN");
    if (client->getStatus() == UNKNOWN)
        return (this->*(this->replies[ERR_NOTREGISTERED]))(message, client);
    else if (client->getStatus() == USER)
        this->infosPerCommand[client->getCurrentCommand()].incrementLocalCount(1);
    else
        this->infosPerCommand[client->getCurrentCommand()].incrementRemoteCount(1);
    if ((check = client->prefixCheck(message)) != "ok")
        return (this->*(this->replies[check]))(message, client);
    
    if (client->getStatus() == USER)
    {
        //join #my,#my2,123
        channelNames = getChannelNames(message.getParameter(0));
        for (int i = 0; i < (int)channelNames.size(); i++)
        {
            fullChannelName = channelNames[i];
            // 이미 유저가 채널에 가입이 되어 있으면 아무일도 일어나지 않음
            if(client->findChannel(fullChannelName))
                continue;
            if(fullChannelName.at(0) == '#' || fullChannelName.at(0) == '&')
            {
                channelName = fullChannelName.substr(1);
                // 채널이 없으면 생성
                if ((it = this->localChannelList.find(fullChannelName)) != this->localChannelList.end())
                    targetChannel = &it->second;
                else if((it = this->remoteChannelList.find(fullChannelName)) != this->remoteChannelList.end())
                    targetChannel = &it->second;
                else
                {
                    this->localChannelList[fullChannelName] = Channel(fullChannelName);
                    targetChannel = &this->localChannelList[fullChannelName];
                }
                // 유저에 채널 리스트에도 채널 추가
                client->joinChannel(targetChannel, fullChannelName);
                this->sendClients[client->getInfo(NICK)].joinChannel(targetChannel, fullChannelName);
                // 채널의 유저 리스트에 채널 추가
                targetChannel->enterUser(client);

                // 같은 채널 안에 있는 유저들에게 메시지 전송
                // 1. local channel user에게 메시지 전송
                // 2. broadcast 전송

                joinedUsers = targetChannel->getUsersList(this->serverName);
                for (int i = 0; i < (int)joinedUsers.size(); i++)
                {
                    if (joinedUsers[i]->getInfo(HOSTNAME) == this->serverName)
                        this->sendMessage(Message(":" + getClientPrefix(client), "JOIN", ":" + fullChannelName), joinedUsers[i]);
                }
                // 353
                this->sendMessage(Message(this->prefix, RPL_NAMREPLY, client->getInfo(NICK) 
                                                                        + " = "
                                                                        + fullChannelName
                                                                        + " :" 
                                                                        + targetChannel->getUserNameList()), client);
                // replis
                // 366
                this->sendMessage(Message(this->prefix, RPL_ENDOFNAMES, client->getInfo(NICK)
                                                                        + " "
                                                                        + fullChannelName
                                                                        + " :End of NAMES list"), client);
                // 다른 서버에 있는 유저들에게 join 는 알리는 메시지
                //this->broadcastMessage(Message(":" + getClientPrefix(client), "JOIN", ":" + fullChannelName), client);
                // 다른 서버에 있는 채널에 유저를 추가하는 메시지
                if (fullChannelName.at(0) == '#')
                    this->broadcastMessage(Message(":" + client->getInfo(NICK), "JOIN", fullChannelName), client);
            }
            // 403 No such channel (채널 이름 오류)
            else
                this->sendMessage(Message(this->prefix, ERR_NOSUCHCHANNEL, client->getInfo(NICK) + " " + fullChannelName + " No such channel"), client);
        }
        // UNKNOWN 일때 451 애러
    }
    else if (client->getStatus() == SERVER)
    {
        std::string     clientName;
        Client          *targetClient;
        // :kmin!2~@loa JOIN :#1
        clientName = message.getPrefix().substr(1);
        targetClient = &this->sendClients[clientName];
        // broadcast 메시지를 받음
        // local channel user를 봐서 있으면 메시지전송
        fullChannelName = message.getParameter(0);
        channelName = message.getParameter(0).substr(1);
        // 현재 서버에 같은 채널의 유저가 있는 경우
        if ((it = this->localChannelList.find(fullChannelName)) != this->localChannelList.end())
            targetChannel = &it->second;
        else if((it = this->remoteChannelList.find(fullChannelName)) != this->remoteChannelList.end())
            targetChannel = &it->second;
        else
        {
            this->remoteChannelList[fullChannelName] = Channel(fullChannelName);
            targetChannel = &this->remoteChannelList[fullChannelName];
        }
        targetClient->joinChannel(targetChannel, message.getParameter(0));
        this->sendClients[targetClient->getInfo(NICK)].joinChannel(targetChannel, fullChannelName);
        joinedUsers = targetChannel->getUsersList(this->serverName);
        for (int i = 0; i < (int)joinedUsers.size(); i++)
            this->sendMessage(Message(getClientPrefix(joinedUsers[i])
                                    , "JOIN"
                                    , ":" + message.getParameter(0))
                                    , joinedUsers[i]);
        targetChannel->enterUser(targetClient);
        this->broadcastMessage(message, client);
    }
    return (CONNECT);
}

int     Server::partHandler(const Message &message, Client *client)
{
    std::string                 channelName;
    std::vector<std::string>    channelNames;
    std::vector<Client *>       joinedUsers;
    Channel                     *targetChannel;

    client->setCurrentCommand("PART");
    // ngircd는 이상하게 2개까지 받음 -> 2812가 2개까지 받음
    if (message.getParameters().size() != 1)
        return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));

    if (client->getStatus() == USER)
    {
        channelNames = getChannelNames(message.getParameter(0));
        for (int i = 0; i < (int)channelNames.size(); i++)
        {
            channelName = channelNames[i];

            if (client->findChannel(channelName))
            {
                targetChannel = &this->localChannelList[channelName];
                
                client->leaveChannel(targetChannel);

                targetChannel->leaveUser(client);
                // 채널에 아무도 없으면 서버에서 채널 삭제
                if (targetChannel->getNumbersOfUsers() == 0)
                {
                    this->localChannelList.erase(channelName);
                    this->sendMessage(Message(getClientPrefix(client), "PART", ":" + channelName), client);
                    // 채널에 혼자 남았을 때 자기 자신에게 part 메시지를 보내고 끝남.
                    continue;
                }
                // 자신 한테도 part 메시지를 날려야 함.
                this->sendMessage(Message(getClientPrefix(client), "PART", ":" + channelName), client);

                joinedUsers = targetChannel->getUsersList("all");
                for (int i = 0; i < (int)joinedUsers.size(); i++)
                    this->sendMessage(Message(getClientPrefix(client), "PART", ":" + channelName), joinedUsers[i]);
                
            }
            // 442
            else if (this->localChannelList.find(channelName) != this->localChannelList.end())
                this->sendMessage(Message(this->prefix, ERR_NOTONCHANNEL, client->getInfo(NICK) + " " + channelName + " :You are not on that channel"), client);
            // 403
            else
                this->sendMessage(Message(this->prefix, ERR_NOSUCHCHANNEL, client->getInfo(NICK) + " " + channelName + " :No such channel"), client);
        }
    }
    //TODO:UNKNOWN 일때 451 애러
    // SERVER 일때 461???
    return (CONNECT);
}

std::string     getTopic(const Message &message)
{
    std::string returnTopic;

    // topic #my :
    // 콜론(:) 만 있고 뒤에 아무것도 없으면 다시 빈 topic 으로 됨.
    if (message.getParameter(1) == ":" && message.getParameters().size() == 2)
    {
        returnTopic = "";
        return (returnTopic);
    }
    if (message.getParameter(1).at(0) == ':')
    {
        returnTopic = message.getParameter(1).substr(1);
        for (int i = 2; i < (int)message.getParameters().size(); i++)
            returnTopic += (" " + message.getParameter(i));
    }
    else
        returnTopic = message.getParameter(1);
    return (returnTopic);
}

int     Server::topicHandler(const Message &message, Client *client)
{   
    std::string             topic;
    std::string             channelName;
    Channel                 *targetChannel = NULL;
    std::map<std::string, Channel>::iterator it;
    std::vector<Client *>   joinedUsers;
    std::time_t             time;

    client->setCurrentCommand("TOPIC");
    // topic #my hello
    // topic #my :  hello 123 1 23 
    if (!((message.getParameters().size() >= 1 && message.getParameters().size() <= 2) 
        || (message.getParameters().size() > 2 && message.getParameter(0).at(0) == ':')))
        return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));

    if (client->getStatus() == USER)
    {   
        channelName = message.getParameter(0);
        // 서버리스트에 채널이 없을 떄
        if (this->localChannelList.find(channelName) == this->localChannelList.end() 
            && this->remoteChannelList.find(channelName) == this->remoteChannelList.end())
        {   // 403
            this->sendMessage(Message(this->prefix, ERR_NOSUCHCHANNEL, client->getInfo(NICK) + " " + channelName + " :No such channel"), client);
            return (CONNECT);
        }

        targetChannel = client->findChannel(channelName);
        // 유저가 채널에 들어가 있지 않을 때
        if (targetChannel == nullptr)
        {
            this->sendMessage(Message(this->prefix, ERR_NOTONCHANNEL, client->getInfo(NICK) + " " + channelName + " :You are not on that channel"), client);
            return(CONNECT);
        }

        // topic 조회 하기
        // topic #my 
        if (message.getParameters().size() == 1)
        {
            if (targetChannel)
            {
                // topic이 없으면
                if (targetChannel->getTopic() == "")    // 331
                    this->sendMessage(Message(this->prefix, RPL_NOTOPIC, client->getInfo(NICK) + " " + channelName + " :No topic is set"), client);
                // topic이 있으면
                else 
                {
                    // 332
                    this->sendMessage(Message(this->prefix, RPL_TOPIC, client->getInfo(NICK) + " " + channelName + " :" + targetChannel->getTopic()), client);

                    time = std::time(0);
                    // 333
                    this->sendMessage(Message(this->prefix, RPL_TOPICWHOTIME, client->getInfo(NICK) + " " + channelName + " " + client->getInfo(NICK) + " " + std::to_string(time)), client);
                }
            }
        }
        else if (message.getParameters().size() >= 2)
        {
            topic = getTopic(message);
            targetChannel->setTopic(topic);
            // 같은 채널에 있는 사람들에게 topic 메시지를 보냄(topic이 바뀌었다고 알림).
            joinedUsers = targetChannel->getUsersList(this->serverName);
            for (int i = 0; i < (int)joinedUsers.size(); i++)
                this->sendMessage(Message(getClientPrefix(client), "TOPIC", channelName + " :" + topic), joinedUsers[i]);
            // 다른 서버에도 보냄.
            if (message.getParameter(0).at(0) == '#')
                this->broadcastMessage(Message(":" + client->getInfo(NICK), "TOPIC", channelName + " :" + topic), client);
        }
    }
    else if (client->getStatus() == SERVER)
    {
        std::string    clientName;
        Client         *targetClient;

        // :song TOPIC #my :hello
        clientName = message.getPrefix().substr(1);
        std::cout << "clientName: " << clientName << std::endl;
        targetClient = &this->sendClients[clientName];
        std::cout << "targetClient: " << targetClient->getInfo(NICK) << std::endl;
        // broadcast 메시지를 받음
        // local channel user를 봐서 있으면 메시지전송
        channelName = message.getParameter(0);
        std::cout << "channelName = " << channelName << std::endl;
        // 현재 서버에 같은 채널의 유저가 있는 경우

        if ((it = this->localChannelList.find(channelName)) != this->localChannelList.end())
            targetChannel = &it->second;
        else if((it = this->remoteChannelList.find(channelName)) != this->remoteChannelList.end())
            targetChannel = &it->second;
        // else
        // {
        //     this->remoteChannelList[channelName] = Channel(channelName);
        //     targetChannel = &this->remoteChannelList[channelName];
        // }
        topic = getTopic(message);
        targetChannel->setTopic(topic);
        std::cout << "targetChannel = " << targetChannel->getName() << std::endl;
        std::cout << "this->serverName = " << this->serverName << std::endl;
        joinedUsers = targetChannel->getUsersList(this->serverName);
        for (int i = 0; i < (int)joinedUsers.size(); i++)
            this->sendMessage(Message(getClientPrefix(joinedUsers[i]), "TOPIC", message.getParameter(0) + " :" + topic), joinedUsers[i]);
        this->broadcastMessage(message, client);
    }
    return (CONNECT);
}
// mode + channel + mode + modeparameter
// 만약 mode만 주어지고 mode parameter가 주어지지않는 경우에는 해당 모드를 가진 list를 출력함
int         Server::modeHandler(const Message &message, Client *client)
{
    std::string					check;
    std::string					from;
    std::vector<std::string>	*list;
    size_t						parameterSize;
    strClientPtrIter			found;
    //Client						*target;

    client->setCurrentCommand("MODE");
    if (client->getStatus() == UNKNOWN)
        return (this->*(this->replies[ERR_NOTREGISTERED]))(message, client);
    else if (client->getStatus() == USER)
        this->infosPerCommand[client->getCurrentCommand()].incrementLocalCount(1);
    else
        this->infosPerCommand[client->getCurrentCommand()].incrementRemoteCount(1);
    if ((check = client->prefixCheck(message)) != "ok")
        return (this->*(this->replies[check]))(message, client);

    parameterSize = message.getParameters().size();
    if (parameterSize > 3 || parameterSize < 2)
        return (this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client);
    else if (parameterSize == 3)
        list = getInfoFromWildcard(message.getParameter(2));
    else
        list = getInfoFromWildcard(this->serverName);
    if (!list)
    {
        delete list;
        return (this->*(this->replies[ERR_NOSUCHSERVER]))(message, client);
    }
    return (CONNECT);
}

int     Server::namesHandler(const Message &message, Client *client)
{
    std::string                                 noChannelUserList;
    std::map<std::string, Channel>::iterator    it;
    std::vector<std::string>                    channelNames;

    client->setCurrentCommand("NAMES");
    if (!(message.getParameters().size() >= 0 && message.getParameters().size() <= 2))
        return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
    
    if (message.getParameters().size() == 0)
    {
        it = this->localChannelList.begin();
        for (; it != this->localChannelList.end(); it++)
            this->sendMessage(Message(this->prefix, RPL_NAMREPLY, client->getInfo(NICK) + " = " + it->first + " :" + it->second.getUserNameList()), client);
        it = this->remoteChannelList.begin();
        for (; it != this->remoteChannelList.end(); it++)
            this->sendMessage(Message(this->prefix, RPL_NAMREPLY, client->getInfo(NICK) + " = " + it->first + " :" + it->second.getUserNameList()), client);
        // 아무 채널에도 속해 있지 않는 유저들 찾기
        strClientPtrIter pit = this->clientList.begin();
        for (; pit != this->clientList.end(); pit++)
        {   
            // TODO: clientList랑 client랑 채널 정보가 다름
            if (pit->second->getNumbersOfJoinedChannels() == 0) 
                noChannelUserList += (pit->first + " ");
        }
        if (!noChannelUserList.empty())
            this->sendMessage(Message(this->prefix, RPL_NAMREPLY, client->getInfo(NICK) + " * * :" + noChannelUserList), client);
        // 366
        this->sendMessage(Message(this->prefix, RPL_ENDOFNAMES, client->getInfo(NICK) + " * :End of NAMES list"), client);
    }
    else if (message.getParameters().size() == 1)
    {
        channelNames = getChannelNames(message.getParameter(0));
        for (int i = 0; i < (int)channelNames.size(); i++)
        {
            if ((it = this->localChannelList.find(channelNames[i])) != this->localChannelList.end())
                this->sendMessage(Message(this->prefix, RPL_NAMREPLY, client->getInfo(NICK) + " = " + it->first + " :" + it->second.getUserNameList()), client);
            else if ((it = this->remoteChannelList.find(channelNames[i])) != this->remoteChannelList.end())
                this->sendMessage(Message(this->prefix, RPL_NAMREPLY, client->getInfo(NICK) + " = " + it->first + " :" + it->second.getUserNameList()), client);
            this->sendMessage(Message(this->prefix, RPL_ENDOFNAMES, client->getInfo(NICK) + " " + channelNames[i] + " :End of NAMES list"), client);            
        }
        
    }    
    return (CONNECT);
}

int     Server::listHandler(const Message &message, Client *client)
{
    std::map<std::string, Channel>::iterator it;
    std::vector<std::string> channelNames;

    client->setCurrentCommand("LIST");
    if (!(message.getParameters().size() >= 0 && message.getParameters().size() <= 2))
        return ((this->*(this->replies[ERR_NEEDMOREPARAMS]))(message, client));
    
    // 321
    this->sendMessage(Message(this->prefix, RPL_LISTSTART, client->getInfo(NICK) + " Channel :Users  Names"), client);

    if (message.getParameters().size() == 0)
    {
        it = this->localChannelList.begin();
        for (; it != this->localChannelList.end(); it++)
            this->sendMessage(Message(this->prefix, RPL_LIST, client->getInfo(NICK) + " " + it->first + " " + std::to_string(it->second.getNumbersOfUsers()) + " :" + it->second.getTopic()), client);
        it = this->remoteChannelList.begin();
        for (; it != this->remoteChannelList.end(); it++)
            this->sendMessage(Message(this->prefix, RPL_LIST, client->getInfo(NICK) + " " + it->first + " " + std::to_string(it->second.getNumbersOfUsers()) + " :" + it->second.getTopic()), client);
        this->sendMessage(Message(this->prefix, RPL_LIST, client->getInfo(NICK) + " &SERVER 0 :Server Messages"), client);
    }
    else if (message.getParameters().size() == 1)
    {
        channelNames = getChannelNames(message.getParameter(0));
        for (int i = 0; i < (int)channelNames.size(); i++)
        {
            if ((it = this->localChannelList.find(channelNames[i])) != this->localChannelList.end())
                this->sendMessage(Message(this->prefix, RPL_LIST, client->getInfo(NICK) + " " + it->first + " " + std::to_string(it->second.getNumbersOfUsers()) + " :" + it->second.getTopic()), client);
            else if ((it = this->remoteChannelList.find(channelNames[i])) != this->remoteChannelList.end())
                this->sendMessage(Message(this->prefix, RPL_LIST, client->getInfo(NICK) + " " + it->first + " " + std::to_string(it->second.getNumbersOfUsers()) + " :" + it->second.getTopic()), client);
        }
    }

    // 323
    this->sendMessage(Message(this->prefix, RPL_LISTEND, client->getInfo(NICK) + " :End of LIST"), client);
    return (CONNECT);
}