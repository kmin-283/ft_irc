#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "utils.hpp"

enum				ClientStatus
{
	UNKNOWN,
	SERVER,
	USER,
	SERVICE
};

enum				UserIndex
{
	PREFIX,
	ORIGINNICK,
	CURRENTNICK,
	USERNAME
};

enum				ServerIndex
{
	UPLINKSERVER,
	SERVERNAME,
	HOPCOUNT,
	SERVERINFO,
};

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

class				Client
{
private:
	int				fd;
	bool			isAuthorized;
	ClientStatus	status;

	std::string		prefix;
	std::string		originNick;
	std::string		currentNick;
	std::string		userName;
	std::string		hostName;
	std::string		serverName;
	std::string		realName;

	std::vector<std::string>	Info;

public:
					Client();
					Client(const int fd, const bool isAuthorized = false);
					~Client(void);
	int				getFd(void) const;
	ClientStatus	getStatus(void) const;
	void			setStatus(const ClientStatus &status);
	bool			getIsAuthorized(void) const;
	void			setIsAuthorized(bool isAuthorized);
	std::string		getOriginNick(void) const;
	void			setOriginNick(const std::string &originNick);
	std::string		getCurrentNick(void) const;
	void			setCurrentNick(const std::string &currentNick);
	void			registerUser(const std::vector<std::string> &parameters);
	bool			isClientRegistered(void);

	void			setInfo(const Message &message, const std::string &myPrefix);
	std::string		getInfo(const int &index) const;

	std::string		getPrefix(void) const;
};

#endif
