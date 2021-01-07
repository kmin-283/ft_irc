#include "Client.hpp"

Client::Client()
	: fd(0), isAuthorized(false), status(UNKNOWN)
{
	this->info.assign(DEFAULT_SIZE, "");
	this->queryData.assign(5, 0);
	this->startTime = std::time(NULL);
}
Client::Client(const int fd, const bool isAuthorized)
	: fd(fd), isAuthorized(isAuthorized), status(UNKNOWN)
{
	this->info.assign(DEFAULT_SIZE, "");
	this->queryData.assign(5, 0);
	this->startTime = std::time(NULL);
}
Client::~Client(void)
{
}

const int &Client::getFd(void) const
{
	return (this->fd);
}

const ClientStatus &Client::getStatus(void) const
{
	return (this->status);
}

void Client::setStatus(const ClientStatus &status)
{
	this->status = status;
}

const bool &Client::getIsAuthorized(void) const
{
	return (this->isAuthorized);
}

void Client::setIsAuthorized(bool isAuthorized)
{
	this->isAuthorized = isAuthorized;
}

void Client::setInfo(const int &index, const std::string &value)
{
	this->info[index] = value;
}

const std::string &Client::getInfo(const int &index) const
{
	return (this->info[index]);
}

const std::vector<std::string> &Client::getInfo(void) const
{
	return (this->info);
}

void Client::incrementQueryData(const int &index, const int &val)
{
	this->queryData[index] += val;
}

std::string Client::getQueryData(const int &index) const
{
	return (std::to_string(this->queryData[index]));
}

std::time_t	Client::getStartTime(void) const
{
	return (this->startTime);
}