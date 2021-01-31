#include "Channel.hpp"

Channel::Channel() {}

Channel::Channel(const std::string& name)
    : name(name) {}

Channel::~Channel() {}

void    Channel::setName(const std::string& name)
{
    this->name = name;
}

const std::string   Channel::getName(void) const
{
    return (this->name);
}

std::vector<Client *> Channel::getUsersList(void)
{
    std::vector<Client *> users;

    std::map<std::string, Client *>::iterator it = this->joinedUsers.begin();
    for (; it != this->joinedUsers.end(); it++)
        users.push_back(it->second);
    return (users);
}

void    Channel::enterUser(Client *user)
{
    this->joinedUsers[user->getInfo(NICK)] = user;
}

void    Channel::leaveUser(Client *user)
{
    this->joinedUsers.erase(user->getInfo(NICK));
}

// 353
std::string Channel::getUserNameList(void)
{
    std::string names;
    std::map<std::string, Client*>::iterator it = this->joinedUsers.begin();
    for (; it != this->joinedUsers.end(); it++)
        names += (it->first + " ");
    return (names);
}

int    Channel::getSize(void)
{
    return (this->joinedUsers.size());
}

void    Channel::showUsersName(void)
{
    std::map<std::string, Client *>::iterator it = this->joinedUsers.begin();

    std::cout << "[ ";
    for (; it != this->joinedUsers.end(); it++)
        std::cout << it->first << " ";
    std::cout << "]" << std::endl;
}