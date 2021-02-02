#include "Client.hpp"

Client::Client()
	: fd(0), isAuthorized(false), status(UNKNOWN), receivedMessageStr("")
	, currentCommand(""), prevCommand(""), waitPong(true)
{
	this->info.assign(DEFAULT_SIZE, "");
	this->queryData.assign(5, 0);
	this->startTime = std::time(NULL);
	this->lastPing = this->startTime;
}
Client::Client(const int fd, const bool isAuthorized)
	: fd(fd), isAuthorized(isAuthorized), status(UNKNOWN), receivedMessageStr("")
	, currentCommand(""), prevCommand(""), waitPong(true)
{
	this->info.assign(DEFAULT_SIZE, "");
	this->queryData.assign(5, 0);
	this->startTime = std::time(NULL);
	this->lastPing = this->startTime;
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

void        Client::setLastPing(const std::time_t current)
{
    this->lastPing = current;
}

std::time_t Client::getLastPing() const
{
    return (std::difftime(std::time(NULL), this->lastPing));
}

void    Client::setWaitPong(bool state)
{
    this->waitPong = state;
}

bool    Client::getWaitPong(void) const
{
    return (this->waitPong);
}

void    Client::setPingLimit(const std::time_t current)
{
    this->pingLimit = current;
}

std::time_t Client::getPingLimit() const
{
    return (std::difftime(std::time(NULL), this->pingLimit));
}

std::string	Client::prefixCheck(const Message &message)
{
	std::string ret;

	if (this->status == USER)
	{
		if (!message.getPrefix().empty())
			return (ERR_UNKNOWNCOMMAND);
	}
	else if (this->status == SERVER)
	{
	    if (message.getCommand() == "PING")
	        return ("ok");
		if (message.getPrefix().empty())
			return (ERR_NEEDMOREPARAMS);
	}
	return ("ok");
}

void				Client::setCurrentCommand(const std::string &command)	
{
	if (this->currentCommand != "")
		this->prevCommand = this->currentCommand;
	this->currentCommand = command;
}
const std::string	&Client::getCurrentCommand(void) const
{
	return (this->currentCommand);
}

const std::string	&Client::getPrevCommand(void) const
{
	return (this->prevCommand);
}

void	Client::joinChannel(Channel *channel)
{
	this->subscribedChannels[channel->getName()] = channel;
}

void	Client::leaveChannel(Channel *channel)
{
	this->subscribedChannels.erase(channel->getName());
}

Channel*	Client::findChannel(std::string channelName)
{
	if (this->subscribedChannels.find(channelName) != this->subscribedChannels.end())
		return (this->subscribedChannels[channelName]);
	else
		return (nullptr);	
}

int			Client::getNumbersOfJoinedChannels(void)
{
	return (this->subscribedChannels.size());
}

void	Client::showChannel(void)
{
	std::map<std::string, Channel *>::iterator it = this->subscribedChannels.begin();
	std::cout << "[ ";
	for (; it != this->subscribedChannels.end(); it++)
		std::cout << it->first << " ";
	std::cout << "] " << std::endl;
}