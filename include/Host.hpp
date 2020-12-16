#ifndef HOST_HPP
# define HOST_HPP

# include "Client.hpp"

class Host : public Client
{
private:
	std::string		prefix;
	std::string		hostName;
	std::string		serverName;
	int				hopcount;
public:
					Host(const Message &message, int fd, bool isAuthorized);
	virtual			~Host(void);
	std::string		getServerName(void) const;
};

#endif
