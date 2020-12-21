#include "Client.hpp"

Client::Client()
	: fd(0), isAuthorized(false), status(UNKNOWN)
{
	this->info.assign(DEFAULT_SIZE, "");
}
Client::Client(const int fd, const bool isAuthorized)
	: fd(fd), isAuthorized(isAuthorized), status(UNKNOWN)
{
	this->info.assign(DEFAULT_SIZE, "");
}
Client::~Client(void)
{}

int							Client::getFd(void) const
{
	return (this->fd);
}

ClientStatus				Client::getStatus(void) const
{
	return (this->status);
}

void						Client::setStatus(const ClientStatus &status)
{
	this->status = status;
}

bool						Client::getIsAuthorized(void) const
{
	return (this->isAuthorized);
}

void						Client::setIsAuthorized(bool isAuthorized)
{
	this->isAuthorized = isAuthorized;
}

void						Client::setInfo(const int &index, const std::string &value)
{
	this->info[index] = value;
}

std::string					Client::getInfo(const int &index) const
{
	return (this->info[index]);
}

std::vector<std::string>	Client::getInfo(void) const
{
	return (this->info);
}
