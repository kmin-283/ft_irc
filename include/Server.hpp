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
	std::string															info;
	const char															*port;
	int																	mainSocket;
	int																	maxFd;
	fd_set																readFds;

	std::map<int, Client>												acceptClients;
	std::map<std::string, Client>										sendClients;
	std::map<std::string, Client *>										serverList;

	std::map<std::string, int (Server::*)(const Message &, Client *)>	commands;

	void																renewFd(const int fd);

	int																	passHandler(const Message &message, Client *client);
	int																	nickHandler(const Message &message, Client *client);
	int																	userHandler(const Message &message, Client *client);
	int																	serverHandler(const Message &message, Client *client);

	void																connectClient(void);
	void																disconnectClient(Client *client);

	void																receiveMessage(const int fd);
	void																sendMessage(const Message &message, Client *client);
	void																broadcastMessage(const Message &message, Client *client);
public:
																		Server(const char *pass, const char *port);
																		~Server(void);
	void																init(void);
	void																start(void);
	void																connectServer(const std::string address);

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
