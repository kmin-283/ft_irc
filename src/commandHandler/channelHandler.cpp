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
    std::string                 channelName;
    std::vector<std::string>    channelNames;
    std::vector<Client *>       joinedUsers;
    Channel                     *targetChannel;

    // 인자 애러 처리 해야함.

    if (client->getStatus() == USER)
    {   
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
                if (this->channelList.find(channelName) == this->channelList.end())
                    this->channelList[channelName] = Channel(channelName);
                targetChannel = &this->channelList[channelName];
                // 유저에 채널 리스트에도 채널 추가
                client->joinChannel(targetChannel);
                // 채널의 유저 리스트에 채널 추가
                targetChannel->enterUser(client);

                // 같은 채널 안에 있는 유저들에게 메시지 전송
                joinedUsers = targetChannel->getUsersList();
                for (int i = 0; i < (int)joinedUsers.size(); i++)
                    this->sendMessage(Message(getClientPrefix(client), "JOIN", ":" + channelName), joinedUsers[i]);

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
                targetChannel = &this->channelList[channelName];
                
                client->leaveChannel(targetChannel);

                targetChannel->leaveUser(client);
                // 채널에 아무도 없으면 서버에서 채널 삭제
                if (targetChannel->getNumbersOfUsers() == 0)
                {
                    this->channelList.erase(channelName);
                    this->sendMessage(Message(getClientPrefix(client), "PART", ":" + channelName), client);
                    // 채널에 혼자 남았을 때 자기 자신에게 part 메시지를 보내고 끝남.
                    continue;
                }
                
                joinedUsers = targetChannel->getUsersList();
                for (int i = 0; i < (int)joinedUsers.size(); i++)
                    this->sendMessage(Message(getClientPrefix(client), "PART", ":" + channelName), joinedUsers[i]);
                
            }
            // 442
            else if (this->channelList.find(channelName) != this->channelList.end())
                this->sendMessage(Message(this->prefix, ERR_NOTONCHANNEL, client->getInfo(NICK) + " " + channelName + " :You are not on that channel"), client);
            // 403
            else
                this->sendMessage(Message(this->prefix, ERR_NOSUCHCHANNEL, client->getInfo(NICK) + " " + channelName + " No such channel"), client);
        }
    }
    // UNKNOWN 일때 451 애러
    // SERVER 일때 461???
    return (CONNECT);
}

