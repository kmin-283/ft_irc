#ifndef SERVER_HPP
# define SERVER_HPP

# include "utils.hpp"
# include "Client.hpp"
# include "Message.hpp"

// typedef void (*commandHandler)(Message &, Client &);
// typedef std::map<std::string, commandHandler> commandMap;
// typedef std::pair<std::string, commandHandler> commandPair;

class														Server
{
private:
	typedef void (Server::*CommandHandler)(Message &, Client &);
	std::string												prefix;
	std::string												pass;
	const char												*port;
	int														mainSocket;
	int														maxFd;
	fd_set													readFds;
	std::map<int, Client*>									acceptClients;
	std::map<std::string, Client*>							sendClients;
	std::map<std::string, CommandHandler>					commands;
	// commandMap												commands;

	struct addrinfo											*getAddrInfo(std::string info);
	void													clearClient(void);
	void													renewFd(const int fd);
	
	void													passHandler(Message &message, Client &client);
	void													nickHandler(Message &message, Client &client);
	void													userHandler(Message &message, Client &client);
public:
															Server(const char *pass, const char *port);
	virtual													~Server(void);
	virtual void											init(void);
	virtual void											acceptConnection(void);
	virtual void											receiveMessage(const int fd);

	void													start(void);
	void													connectServer(const std::string address);

	std::string												getPass(void) const;
	int														getSocket(void) const;

	class GetAddressFailException: public std::exception
	{
		virtual const char* what() const throw();
	};
	class SocketOpenFailException: public std::exception
	{
		virtual const char* what() const throw();
	};
	class SocketBindFailException: public std::exception
	{
		virtual const char* what() const throw();
	};
	class SocketListenFailException: public std::exception
	{
		virtual const char* what() const throw();
	};
	class SelectFailException: public std::exception
	{
		virtual const char* what() const throw();
	};
	class AcceptFailException: public std::exception
	{
		virtual const char* what() const throw();
	};
	class ReceiveMessageFailException: public std::exception
	{
		virtual const char* what() const throw();
	};
};

#endif
