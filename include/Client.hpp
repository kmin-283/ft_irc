#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "utils.hpp"

enum				ClientStatus
{
	UNKNOWN,
	// AUTHORIZED,
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
public:
					Client(const int fd, const bool isAuthorized = false);
					~Client(void);
	int				getFd(void) const;
	ClientStatus	getStatus(void) const;
	void			setStatus(const ClientStatus &status);
	bool			getIsAuthorized(void) const;
	void			setIsAuthorized(bool isAuthorized);
};

#endif
