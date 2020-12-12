#include "Client.hpp"

Client::Client(const int fd)
	: fd(fd), status(UNKNOWN)
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
