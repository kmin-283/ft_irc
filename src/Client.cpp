#include "Client.hpp"
Client::Client()
{}
Client::Client(const int fd, const bool isAuthorized)
	: fd(fd), isAuthorized(isAuthorized), status(UNKNOWN)
{}
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

// void						Client::setInfo(const Message &message, const std::string &myPrefix)
// void						Client::setInfo(const Message &message, const std::string &myPrefix)
void						Client::setInfo(const int &index, const std::string &value)
{
	this->info[index] = value;
	std::cout << "set INFO" << this->info[index] << std::endl;
	// this->setStatus(SERVER);
	// this->Info.assign(4, "");
	
	// // 최 상위 uplink인 경우 irc.example.net 이면 uplink를 ""으로 할 것인가?
		
	// this->Info[UPLINKSERVER] = message.getPrefix();
	
	// if (message.getPrefix() == "")
	// 	this->Info[UPLINKSERVER] = myPrefix;
	// this->Info[SERVERNAME] = message.getParameter(0);
	// this->Info[HOPCOUNT] = message.getParameter(1);
	// this->Info[SERVERINFO] = message.getParameter(2);

	// // std::cout <<this->Info[UPLINKSERVER] << " " << this->Info[SERVERNAME]
	// // << " " << this->Info[HOPCOUNT] << " " << this->Info[SERVERINFO] << std::endl;
}

std::string					Client::getInfo(const int &index) const
{
	return (this->info[index]);
}

std::vector<std::string>	Client::getInfo(void) const
{
	return (this->info);
}