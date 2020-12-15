#ifndef SERVER_HPP
# define SERVER_HPP

# include "utils.hpp"
# include "Client.hpp"
# include "Message.hpp"
# include "NumericReplies.hpp"

class																	Server
{
private:
	std::string															prefix;
	std::string															pass;
	const char															*port;
	int																	mainSocket;
	int																	maxFd;
	fd_set																readFds;
	std::map<int, Client*>												acceptClients;
	std::map<std::string, Client*>										sendClients;
	std::map<std::string, void (Server::*)(const Message &, Client &)>	commands;

	struct addrinfo														*getAddrInfo(const std::string info);
	void																clearClient(void);
	void																renewFd(const int fd);

	void																passHandler(const Message &message, Client &client);
	void																nickHandler(const Message &message, Client &client);
	void																userHandler(const Message &message, Client &client);

	void																sendNumericReplies(const std::string &numericReplies, const std::string &replyString, Client &client);
public:
																		Server(const char *pass, const char *port);
	virtual																~Server(void);
	virtual void														init(void);
	virtual void														acceptConnection(void);
	virtual void														receiveMessage(const int fd);

	void																start(void);
	void																connectServer(const std::string address);

	std::string															getPass(void) const;
	int																	getSocket(void) const;

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
