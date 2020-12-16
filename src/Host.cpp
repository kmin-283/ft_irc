#include "Host.hpp"

Host::Host(const Message &message, int fd, bool isAuthorized)
    : Client(fd, isAuthorized)
{
	this->prefix = ":" + message.getParameter(0);
	this->hostName = message.getParameter(0);
	this->serverName = this->hostName;
	this->hopcount = ft_atoi(message.getParameter(1).c_str());
}

Host::~Host(void)
{}

std::string			Host::getServerName(void) const
{
	return (this->serverName);
}
