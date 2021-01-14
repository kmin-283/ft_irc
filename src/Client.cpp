#include "Client.hpp"

Client::Client()
	: fd(0), isAuthorized(false), status(UNKNOWN), receivedMessageStr("")
{
	this->info.assign(DEFAULT_SIZE, "");
	this->queryData.assign(5, 0);
	this->startTime = std::time(NULL);
}
Client::Client(const int fd, const bool isAuthorized)
	: fd(fd), isAuthorized(isAuthorized), status(UNKNOWN), receivedMessageStr("")
{
	this->info.assign(DEFAULT_SIZE, "");
	this->queryData.assign(5, 0);
	this->startTime = std::time(NULL);
}
Client::~Client(void)
{
}

Client						&Client::operator=(const Client &client)
{
	this->fd = client.fd;
	this->isAuthorized = client.isAuthorized;
	this->status = client.status;
	this->info.resize(client.info.size());
	std::copy(client.info.begin(), client.info.end(), this->info.begin());
	return (*this);
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

std::string	Client::getReceivedMessageStr(void) const
{
	return (this->receivedMessageStr);
}

void		Client::addReceivedMessageStr(char buffer)
{
	this->receivedMessageStr += buffer;
}

void		Client::clearReceivedMessageStr(void)
{
	this->receivedMessageStr.clear();
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
