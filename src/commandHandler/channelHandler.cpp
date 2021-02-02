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
            channelName = channelNames[i];

            // 이미 유저가 채널에 가입이 되어 있으면 아무일도 일어나지 않음
            if(client->findChannel(channelName))
                continue;
            if(channelName.at(0) == '#' || channelName.at(0) == '&')
            {   
                // 채널이 없으면 생성
                if ((it = this->localChannelList.find(channelName)) != this->localChannelList.end())
                    targetChannel = &it->second;
                else if((it = this->remoteChannelList.find(channelName)) != this->remoteChannelList.end())
                    targetChannel = &it->second;
                else
                {
                    this->localChannelList[channelName] = Channel(channelName);
                    targetChannel = &this->localChannelList[channelName];
                }
                // 유저에 채널 리스트에도 채널 추가
                client->joinChannel(targetChannel);
                // 채널의 유저 리스트에 채널 추가
                targetChannel->enterUser(client);

                // 같은 채널 안에 있는 유저들에게 메시지 전송
                // 1. local channel user에게 메시지 전송
                // 2. broadcast 전송
                joinedUsers = targetChannel->getUsersList("all");
                for (int i = 0; i < (int)joinedUsers.size(); i++)
                    this->sendMessage(Message(":" + getClientPrefix(client), "JOIN", ":" + channelName), joinedUsers[i]);
                // 다른 서버에도 보냄.
                this->broadcastMessage(Message(":" + client->getInfo(NICK), "JOIN", ":" + channelName), client);
                // 353
                this->sendMessage(Message(this->prefix, RPL_NAMREPLY, client->getInfo(NICK) 
                                                                        + " = "
                                                                        + targetChannel->getName()
                                                                        + " :" 
                                                                        + targetChannel->getUserNameList()), client);
                // replis
                // 366
                this->sendMessage(Message(this->prefix, RPL_ENDOFNAMES, client->getInfo(NICK)
                                                                        + " "
                                                                        + targetChannel->getName()
                                                                        + " :End of NAMES list"), client);
            }
            // 403 No such channel (채널 이름 오류)
            else
                this->sendMessage(Message(this->prefix, ERR_NOSUCHCHANNEL, client->getInfo(NICK) + " " + channelName + " No such channel"), client);
        }
        // UNKNOWN 일때 451 애러
    }
    else if (client->getStatus() == SERVER)
    {
         std::string    clientName;
         Client         *targetClient;
         size_t         idx;
         // :kmin!2~@loa JOIN :#1
         if ((idx = message.getPrefix().find("!", 0)) != std::string::npos)
             clientName = message.getPrefix().substr(1, idx-1);
         else
             clientName = message.getPrefix().substr(1, message.getPrefix().length());
         targetClient = &this->sendClients[clientName];
         // broadcast 메시지를 받음
         // local channel user를 봐서 있으면 메시지전송
         channelName = message.getParameter(0).substr(1, message.getParameter(0).length());
         // 현재 서버에 같은 채널의 유저가 있는 경우

         if ((it = this->localChannelList.find(channelName)) != this->localChannelList.end())
             targetChannel = &it->second;
         else if((it = this->remoteChannelList.find(channelName)) != this->remoteChannelList.end())
             targetChannel = &it->second;
         else
         {
             this->remoteChannelList[channelName] = Channel(channelName);
             targetChannel = &this->remoteChannelList[channelName];
         }
         targetClient->joinChannel(targetChannel);
         if (idx != std::string::npos)
         {
             joinedUsers = targetChannel->getUsersList(this->serverName);
             for (int i = 0; i < (int)joinedUsers.size(); i++)
                 this->sendMessage(message, joinedUsers[i]);
         }
         this->broadcastMessage(message, client);
         targetChannel->enterUser(targetClient);
    }
    return (CONNECT);
}

int     Server::partHandler(const Message &message, Client *client)
{
    std::string                 channelName;
    std::vector<std::string>    channelNames;
    std::vector<Client *>       joinedUsers;
    Channel                     *targetChannel;

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
    Channel                 *targetChannel;
    std::vector<Client *>   joinedUsers;
    std::time_t             time;

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
            // 같은 채넣에 있는 사람들에게 topic 메시지를 보냄(topic이 바뀌었다고 알림).
            joinedUsers = targetChannel->getUsersList("all");
            for (int i = 0; i < (int)joinedUsers.size(); i++)
                this->sendMessage(Message(getClientPrefix(client), "TOPIC", channelName + " :" + topic), joinedUsers[i]);
            //TODO: 다른 서버 같은 채널에 있는 사람들에게도 topic이 변경 된 메시지를 받아야 함.
                    
            //TODO: 다른 서버에 있는 채널들에 topic도 변경 되어야 함.
        }
    }
    return (CONNECT);
}