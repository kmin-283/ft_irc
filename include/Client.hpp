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

class				Client
{
private:
	int				fd;
	ClientStatus	status;
	bool			isAuthorized;
	std::string		originNick;
	std::string		currentNick;
	std::string		prefix;
public:
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
};

#endif
