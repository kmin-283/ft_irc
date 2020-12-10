#include "Client.hpp"

Client::Client(const size_t fd)
	: fd(fd)
{
}

Client::~Client(void)
{
}

int				Client::getFd(void) const
{
	return (this->fd);
}
