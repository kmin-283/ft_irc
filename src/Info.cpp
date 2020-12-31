#include "Server.hpp"

Server::Info::Info()
    : bytes(0), requestCount(0), mediationCount(0)
{
}
Server::Info::~Info()
{
}
inline void Server::Info::setBytes(const int &size)
{
    this->bytes += size;
}
inline std::string Server::Info::getBytes() const
{
    return (std::to_string(this->bytes));
}
inline void Server::Info::setRequestCount(const int &size)
{
    this->requestCount += size;
}
inline std::string Server::Info::getRequestCount() const
{
    return (std::to_string(this->requestCount));
}
inline void Server::Info::setMediationCount(const int &size)
{
    this->mediationCount += size;
}
inline std::string Server::Info::getMediationCount() const
{
    return (std::to_string(this->mediationCount));
}
inline std::string Server::Info::getCmd() const
{
    return (this->cmd);
}