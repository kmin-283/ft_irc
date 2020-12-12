#ifndef TLS_SERVER_HPP
# define TLS_SERVER_HPP

# include "Server.hpp"

class				TLSServer : public Server
{
	public:
					TLSServer(const char *pass);
	virtual			~TLSServer(void);
	virtual void	init(const char *port);
	virtual void	acceptConnection(void);
	virtual void	receiveMessage(const int fd);
};

#endif
