#include "Channel.hpp"

Channel::Channel() {}

Channel::Channel(const std::string& name)
	: name(name.substr(1, name.length())), topic(""), mode(0)
{
	(void)mode;
}
Channel::~Channel() {}

void    Channel::setName(const std::string& name)
{
	this->name = name;
}

const std::string   &Channel::getName(void) const
{
	return (this->name);
}

void    Channel::setTopic(const std::string& topic)
{
	this->topic = topic;
}

const std::string   &Channel::getTopic(void) const
{
	return (this->topic);
}

void 				Channel::setMode(const int &mode, const bool &setConfig)
{
	if (setConfig)
		this->mode |= mode;
	else
		this->mode ^= mode;
}

void 				Channel::setKey(const std::string &key)
{
	this->key = key;
	if (!(this->mode & MODE_K))
		this->setMode(MODE_K, ON);
}

void 				Channel::clearKey(void)
{
	this->key.clear();
	if (this->mode & MODE_K)
		this->setMode(MODE_K, OFF);
}

void 				Channel::setLimit(const std::string &limit)
{
	this->maximumUser = ft_atoi(limit.c_str());
	if (!(this->mode & MODE_L))
		this->setMode(MODE_L, ON);
}

void 				Channel::clearLimit()
{
	this->maximumUser = INT_MAX;
	if (this->mode & MODE_L)
		this->setMode(MODE_L, OFF);
}
void 				Channel::setList(std::set<std::string> &list, const int &mode, const std::string &mask)
{
	list.insert(mask);
	if (!(this->mode & mode))
		this->setMode(mode, ON);
}

std::set<std::string> &Channel::getBanList(void)
{
	return (this->banList);
}

void 				Channel::eraseElement(std::set<std::string> &list, const int &mode, const std::string &mask)
{
	list.erase(mask);
	if (list.empty())
		this->setMode(mode, OFF);
}

void 				Channel::clearList(std::set<std::string> &list, const int &mode)
{
	list.clear();
	if (this->mode & mode)
		this->setMode(mode, OFF);
}
std::set<std::string> &Channel::getExceptionList(void)
{
	return (this->exceptionList);
}
std::set<std::string> &Channel::getInvitationList(void)
{
	return (this->invitationList);
}

const int 			&Channel::getMode() const
{
	return (this->mode);
}

int					Channel::toggleMode(std::string &successState, std::string &error, size_t &modeIndex, const Message &message)
{
	bool        isAdd = true;
	char        mode;
	size_t      modeParamIndex;
	size_t      modeCharIndex = 0;
	std::map<std::string, Client *>::iterator it;

	if (message.getParameter(modeIndex)[0] == '-')
		isAdd = false;
	successState = "+";
	if (!isAdd)
		successState = "-";
	if (message.getParameter(modeIndex)[0] == '-' || message.getParameter(modeIndex)[0] == '+')
		modeCharIndex += 1;
	mode = message.getParameter(modeIndex)[modeCharIndex];
	modeParamIndex = modeIndex + 1;
	while (mode != 0)
	{
		switch (mode) {
			case 'a':
				break;

			case 'i':
				if (isAdd)
				{
					if (this->mode & MODE_LI)
						break;
					this->mode ^= MODE_LI;
				}
				else
				{
					if (this->mode ^ MODE_LI)
						break;
					this->mode ^= MODE_LI;
				}
				successState += "i";
				break;

			case 'm':
				if (isAdd)
				{
					if (this->mode & MODE_M)
						break;
					this->mode ^= MODE_M;
				}
				else
				{
					if (this->mode ^ MODE_M)
						break;
					this->mode ^= MODE_M;
				}
				successState += "m";
				break;

			case 'n':
				if (isAdd)
				{
					if (this->mode & MODE_N)
						break;
					this->mode ^= MODE_N;
				}
				else
				{
					if (this->mode ^ MODE_N)
						break;
					this->mode ^= MODE_N;
				}
				successState += "n";
				break;

			case 'q':
				if (isAdd)
				{
					if (this->mode & MODE_Q)
						break;
					this->mode ^= MODE_Q;
				}
				else
				{
					if (this->mode ^ MODE_Q)
						break;
					this->mode ^= MODE_Q;
				}
				successState += "q";
				break;

			case 'p':
				if (isAdd)
				{
					if (this->mode & MODE_P)
						break;
					this->mode ^= MODE_P;
				}
				else
				{
					if (this->mode ^ MODE_P)
						break;
					this->mode ^= MODE_P;
				}
				successState += "p";
				break;

			case 's':
				if (isAdd)
				{
					if (this->mode & MODE_S)
						break;
					this->mode ^= MODE_S;
				}
				else
				{
					if (this->mode ^ MODE_S)
						break;
					this->mode ^= MODE_S;
				}
				successState += "s";
				break;

			case 'r':
				break;

			case 't':
				if (isAdd)
				{
					if (this->mode & MODE_T)
						break;
					this->mode ^= MODE_T;
				}
				else
				{
					if (this->mode ^ MODE_T)
						break;
					this->mode ^= MODE_T;
				}
				successState += "t";
				break;
			case 'o':
				break;
			case 'v':
				break;
			case 'k':
				break;
			case 'l':
				break;
			case 'b':
				break;
			case 'e':
				break;
			case 'I':
				break;
			default:
				error = ERR_UNKNOWNMODE;
				return (-1);
		}
		mode = message.getParameter(modeIndex)[++modeCharIndex];
	}
	modeIndex = modeParamIndex + 1;
	return (0);
}

Client                  *Channel::findOperator(const std::string &userName)
{
	for (strClientIter it = this->operators.begin(); it != this->operators.end(); ++it)
	{
		if (userName == it->second->getInfo(NICK))
			return (it->second);
	}
	return (NULL);
}

std::map<std::string, Client *>	&Channel::getOperators(void)
{
	return (this->operators);
}

Client	*Channel::findJoinedUser(const std::string &userName)
{
	for (strClientIter it = this->joinedUsers.begin(); it != this->joinedUsers.end(); it++)
	{	
		if (userName == it->second->getInfo(NICK))
			return (it->second);
	}
	return (NULL);
}

std::vector<Client *> Channel::getUsersList(const std::string &mask)
{
	std::vector<Client *> users;
	std::map<std::string, Client *>::iterator it = this->operators.begin();

	for (; it != this->operators.end(); it++)
	{
		if (mask == "all" || it->second->getInfo(HOSTNAME) == mask)
			users.push_back(it->second);
	}
	it = this->joinedUsers.begin();
	for (; it != this->joinedUsers.end(); it++)
	{
		if (mask == "all" || it->second->getInfo(HOSTNAME) == mask)
			users.push_back(it->second);
	}
	return (users);
}

void    Channel::enterUser(Client *user)
{
	// 무조건 처음 들어온 유저는 관리자가 됨.
	if (this->getNumbersOfUsers() == 0)
		this->operators[user->getInfo(NICK)] = user;
	else
		this->joinedUsers[user->getInfo(NICK)] = user;
}

void    Channel::leaveUser(Client *user)
{
	// 관리자에 있으면 관리자에서 삭제
	if (this->operators.find(user->getInfo(NICK)) != this->operators.end())
		this->operators.erase(user->getInfo(NICK));
	else
		this->joinedUsers.erase(user->getInfo(NICK));
}

// 353
std::string Channel::getUserNameList(void)
{
	std::string returnNames;
	// 역순으로 출력 되기 때문에 리버스 반복자 사용 -> 그런데 map 이라서 알파벳 역순으로 나옴.. -> ngircd 랑 순서 다름.
	// 일반 유저 먼저
	std::map<std::string, Client*>::reverse_iterator rit = this->joinedUsers.rbegin();
	for (; rit != this->joinedUsers.rend(); rit++)
		returnNames += (rit->first + " ");
	rit = this->operators.rbegin();
	for (; rit != this->operators.rend(); rit++)
		returnNames += ("@" + rit->first + " ");
	return (returnNames);
}

int    Channel::getNumbersOfUsers(void)
{
	return (this->operators.size() + this->joinedUsers.size());
}

void    Channel::showUsersName(void)
{
	std::map<std::string, Client *>::iterator it = this->operators.begin();

	std::cout << "[ ";
	for (; it != this->operators.end(); it++)
		std::cout << "@" <<it->first << " ";
	it = this->joinedUsers.begin();
	for (; it != this->joinedUsers.end(); it++)
		std::cout << it->first << " ";
	std::cout << "]" << std::endl;
}

void 								Channel::makeUserToOper(const std::string &operName)
{
	if (!this->operators.count(operName))
	{
		this->operators[operName] = this->joinedUsers[operName];
		this->joinedUsers.erase(operName);
	}
}
void 								Channel::makeOperToUser(const std::string &userName)
{
	if (!this->joinedUsers.count(userName))
	{
		this->joinedUsers[userName] = this->operators[userName];
		this->operators.erase(userName);
	}
}
// channelPrefix   &Channel::getChannelPrefix(void) const
// {
//     return (this->chPrefix);
// }