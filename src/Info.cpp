#include "Server.hpp"

Server::Info::Info()
    : bytes(0), remoteCount(0), localCount(0)
{
}
Server::Info::~Info()
{
}
void Server::Info::incrementBytes(const int &size)
{
    this->bytes += size;
}
std::string Server::Info::getBytes() const
{
    return (std::to_string(this->bytes));
}
void Server::Info::incrementRemoteCount(const int &size)
{
    this->remoteCount += size;
}
std::string Server::Info::getRemoteCount() const
{
    return (std::to_string(this->remoteCount));
}
void Server::Info::incrementLocalCount(const int &size)
{
    this->localCount += size;
}
std::string Server::Info::getLocalCount() const
{
    return (std::to_string(this->localCount));
}