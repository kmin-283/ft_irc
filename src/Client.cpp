#include "Client.hpp"

Client::Client(const int fd, const bool isAuthorized)
	: fd(fd), status(UNKNOWN), isAuthorized(isAuthorized)
{
}
 
Client::~Client(void)
{
}

int				Client::getFd(void) const
{
	return (this->fd);
}


ClientStatus	Client::getStatus(void) const
{
	return (this->status);
}

void			Client::setStatus(const ClientStatus &status)
{
	this->status = status;
}

bool			Client::getIsAuthorized(void) const
{
	return (this->isAuthorized);
}

void			Client::setIsAuthorized(bool isAuthorized)
{
	this->isAuthorized = isAuthorized;
}