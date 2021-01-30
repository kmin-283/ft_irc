#include "Server.hpp"

int     Server::joinHandler(const Message &message, Client *client)
{
    std::string         channelName;
    Channel             *targetChannel;

    if (client->getStatus() == USER)
    {
        if (message.getParameter(0).at(0) == '#')
        {   
            channelName = message.getParameter(0);
            // 서버의 채널 리스트에 만들려고 하는 채널이 있는 지 확인
            // 있으면 기존 채널 사
            if(this->channelList.find(channelName) != this->channelList.end())
            // iterator를 만들어서 참조!
                targetChannel = &this->channelList[channelName];
            // 없으면 채널 새로 만들기 
            else
            {
                this->channelList[channelName] = Channel(channelName);
                targetChannel = &this->channelList[channelName];
            }
            // 유저의 가입 채널 리스트에 채널 추가
            client->joinChannel(targetChannel);
            // 채널의 유저 리스트에도 유저 추가 
            targetChannel->enterUser(client);
            // 같은 채널 안에 있는 유저들에게 메시지 전송

            // 353

            // 366
        }
    }
    
    return (CONNECT);
}