#include "Channel.hpp"

Channel::Channel() {}

Channel::Channel(const std::string& name)
    : name(name), topic("") {}

Channel::~Channel() {}

void    Channel::setName(const std::string& name)
{
    this->name = name;
}

const std::string   &Channel::getName(void) const
{
    return (this->name);
}

void    Channel::setTopic(const std::string& topic)
{
    this->topic = topic;
}

const std::string   &Channel::getTopic(void) const
{
    return (this->topic);
}


std::vector<Client *> Channel::getUsersList(void)
{
    std::vector<Client *> users;
    
    std::map<std::string, Client *>::iterator it = this->operators.begin();
    for (; it != this->operators.end(); it++)
        users.push_back(it->second);
    
    it = this->joinedUsers.begin();
    for (; it != this->joinedUsers.end(); it++)
        users.push_back(it->second);
    return (users);
}

void    Channel::enterUser(Client *user)
{
    // 무조건 처음 들어온 유저는 관리자가 됨.
    if (this->getNumbersOfUsers() == 0)
        this->operators[user->getInfo(NICK)] = user;
    else
        this->joinedUsers[user->getInfo(NICK)] = user;
}

void    Channel::leaveUser(Client *user)
{
    // 관리자에 있으면 관리자에서 삭제
    if (this->operators.find(user->getInfo(NICK)) != this->operators.end())
        this->operators.erase(user->getInfo(NICK));
    else
        this->joinedUsers.erase(user->getInfo(NICK));
}

// 353
std::string Channel::getUserNameList(void)
{   
    std::string returnNames;
    // 역순으로 출력 되기 때문에 리버스 반복자 사용 -> 그런데 map 이라서 알파벳 역순으로 나옴.. -> ngircd 랑 순서 다름.
    // 일반 유저 먼저
    std::map<std::string, Client*>::reverse_iterator rit = this->joinedUsers.rbegin();
    for (; rit != this->joinedUsers.rend(); rit++)
        returnNames += (rit->first + " ");
    rit = this->operators.rbegin();
    for (; rit != this->operators.rend(); rit++)
        returnNames += ("@" + rit->first + " ");
    return (returnNames);
}

int    Channel::getNumbersOfUsers(void)
{
    return (this->operators.size() + this->joinedUsers.size());
}

void    Channel::showUsersName(void)
{
    std::map<std::string, Client *>::iterator it = this->operators.begin();

    std::cout << "[ ";
    for (; it != this->operators.end(); it++)
        std::cout << "@" <<it->first << " ";
    it = this->joinedUsers.begin();
    for (; it != this->joinedUsers.end(); it++)
        std::cout << it->first << " ";
    std::cout << "]" << std::endl;
}