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
public:
					Client(const int fd);
					~Client(void);
	int				getFd(void) const;
	ClientStatus	getStatus(void) const;
};

#endif
