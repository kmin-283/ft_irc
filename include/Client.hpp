#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "utils.hpp"
#include "Replies.hpp"
#include "Channel.hpp"

class Channel;

enum ClientStatus
{
	UNKNOWN,
	SERVER,
	USER,
	SERVICE
};

enum UserIndex
{
	HOSTNAME,
	NICK,
	ADDRESS,
	USERNAME,
	REALNAME,
	USERMODE
};

enum ServerIndex
{
	UPLINKSERVER,
	SERVERNAME,
	HOPCOUNT,
	SERVERINFO,
	TOKEN,
};

enum QueryData
{
	SENDQUEUE,
	SENDMSG,
	SENDBYTES,
	RECVMSG,
	RECVBYTES,
};
// D -- A -- B -- C

// : A SERVER D 1 :123123

// client A
// : A SERVER D 2 :123123
// client B
// : A SERVER D 3 :123123

// Ng - Sa - Sb
// Sa - Ng - Sb

// Sa - Sb - Sc - Sd - Se
//		|
// 		Sf
// (Sc)
// Sc fd(Sb = fd)
// Sc Sb prefix, Hostname,ServerName

// Sa
//  |
// / \
// Sb Sc
// /
// Sd
// Client(fd)	-> User -> User
// 			-> Host -> Host

//  Sa - Sb - Sc(:irc.example.net SERVER localhost.6671 2 3 : 123123/r/n) - Cc
// 	|	 |
// 	Ca	 Cb (NICK dakim :1/r/n:dakim USER ~dakim localhost irc.example.net :123/r/n)

// Cb -> fd Sb / prefix Cb
// Sc -> fd Sb / prefix Sc

// :dakim1!~dakim1@localhost PRIVMSG dakim :hello (Ca)
// :dakim PRIVMSG dakim1 :hello (Cb)
// :localhost.6671 PRIVMSG dakim2 :hello (Cc)

class Client
{
private:
	int 								fd;
	bool								isAuthorized;

	ClientStatus 						status;
	std::string							receivedMessageStr;
	std::vector<std::string> 			info;
	std::vector<size_t> 				queryData;

	std::string			currentCommand;
	std::string			prevCommand;

    std::time_t			startTime;
    bool                waitPong;
    std::time_t         lastPing;
    std::time_t         pingLimit;
	
	std::map<std::string, Channel *>	subscribedChannels;

public:
	Client(void);
	Client(const int fd, const bool isAuthorized = false);
	~Client(void);
	Client &operator=(const Client &client);
	const int &getFd(void) const;
	const ClientStatus &getStatus(void) const;
	void setStatus(const ClientStatus &status);
	const bool &getIsAuthorized(void) const;
	void setIsAuthorized(bool isAuthorized);
	void setInfo(const int &index, const std::string &myPrefix);
	std::string		getReceivedMessageStr() const;
	void			addReceivedMessageStr(char buffer);
	void			clearReceivedMessageStr(void);

	const std::string &getInfo(const int &index) const;
	const std::vector<std::string> &getInfo(void) const;

	void incrementQueryData(const int &index, const int &val);
	std::string getQueryData(const int &index) const;

	std::time_t	getStartTime(void) const;
	void        setLastPing(const std::time_t current);
	std::time_t getLastPing(void) const;

	void        setWaitPong(bool state);
	bool        getWaitPong(void) const;

	void        setPingLimit(const std::time_t current);
	std::time_t getPingLimit() const;

	std::string prefixCheck(const Message &message);

	void				setCurrentCommand(const std::string &command);
	const std::string	&getCurrentCommand(void) const;
	const std::string	&getPrevCommand(void) const;
	void								joinChannel(Channel *channel);
	void								leaveChannel(Channel *channel);
	Channel*							findChannel(std::string channelName);
	int									getNumbersOfJoinedChannels(void);

	// 디버깅
	void								showChannel(void);

};

#endif
