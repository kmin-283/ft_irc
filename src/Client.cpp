#include "Client.hpp"

Client::Client(const int fd, const bool isAuthorized)
	: fd(fd), isAuthorized(isAuthorized)
{
}

Client::~Client(void)
{
}

int				Client::getFd(void) const
{
	return (this->fd);
}


// ClientStatus	Client::getStatus(void) const
// {
// 	return (this->status);
// }

// void			Client::setStatus(const ClientStatus &status)
// {
// 	this->status = status;
// }

bool			Client::getIsAuthorized(void) const
{
	return (this->isAuthorized);
}

void			Client::setIsAuthorized(bool isAuthorized)
{
	this->isAuthorized = isAuthorized;
}

// std::string		Client::getOriginNick(void) const
// {
// 	return (this->originNick);
// }

// void			Client::setOriginNick(const std::string &originNick)
// {
// 	this->originNick = originNick;
// }

// std::string		Client::getCurrentNick(void) const
// {
// 	return (this->currentNick);
// }

// void			Client::setCurrentNick(const std::string &currentNick)
// {
// 	this->currentNick = currentNick;
// }

// void			Client::registerUser(const std::vector<std::string> &parameters)
// {
// 	int			i;
// 	std::string *userValue[4];

// 	i = 0;
// 	userValue[0] = &this->userName;
// 	userValue[1] = &this->hostName;
// 	userValue[2] = &this->serverName;
// 	userValue[3] = &this->realName;
// 	for(std::vector<std::string>::const_iterator iterator = parameters.begin(); iterator != parameters.end(); ++i, ++iterator)
// 		*(userValue[i]) = *iterator;
// }

// bool			Client::isClientRegistered(void)
// {
// 	std::string	*clientValues[5];

// 	clientValues[0] = &this->userName;
// 	clientValues[1] = &this->hostName;
// 	clientValues[2] = &this->serverName;
// 	clientValues[3] = &this->realName;
// 	clientValues[4] = &this->currentNick;
// 	if (this->status != UNKNOWN)
// 		return (false);
// 	if (!this->isAuthorized)
// 		return (false);
// 	for(int i = 0; i < 5; ++i)
// 		if (*(clientValues[i]) == "")
// 			return (false);
// 	return (true);
// }


