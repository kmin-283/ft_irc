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
	ORIGINNICK,
	CURRENTNICK,
	USERNAME,
	REALNAME,
	HOSTNAME
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
	int							fd;
	bool						isAuthorized;
	ClientStatus				status;
	std::vector<std::string>	info;
public:
								Client(void);
								Client(const int fd, const bool isAuthorized = false);
								~Client(void);

	int							getFd(void) const;
	ClientStatus				getStatus(void) const;
	void						setStatus(const ClientStatus &status);
	bool						getIsAuthorized(void) const;
	void						setIsAuthorized(bool isAuthorized);
	void						setInfo(const int &index, const std::string &myPrefix);
	std::string					getInfo(const int &index) const;
	std::vector<std::string>	getInfo(void) const;
};

#endif
