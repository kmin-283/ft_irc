#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "utils.hpp"

class				Client
{
private:
	size_t			fd;
public:
					Client(const size_t fd);
					~Client(void);
	int				getFd(void) const;
};

#endif
