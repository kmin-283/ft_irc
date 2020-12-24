#ifndef SERVER_HPP
# define SERVER_HPP

# include "utils.hpp"
# include "Client.hpp"
# include "Channel.hpp"
# include "Message.hpp"
# include "Replies.hpp"

class																	Server
{
private:
	std::string															prefix;

	std::string															ipAddress;
	std::string															serverName;
	std::string															version;
	std::string															startTime;
	std::string															userMode;
	std::string															channelMode;
	std::string															motdDir;

	std::string															pass;
	std::string															info;
	const char															*port;
	int																	mainSocket;
	int																	maxFd;
	fd_set																readFds;

	std::map<int, Client>												acceptClients;
	std::map<std::string, Client>										sendClients;
	std::map<std::string, Client *>										serverList;
	std::map<std::string, Client *>										clientList;
	std::map<std::string, Client *>										serviceList;
	std::map<std::string, Channel>										channelList;

	std::map<std::string, int (Server::*)(const Message &, Client *)>	commands;
	void																registerCommands(void);
	int																	passHandler(const Message &message, Client *client);
	int																	nickHandler(const Message &message, Client *client);
	int																	userHandler(const Message &message, Client *client);
	int																	serverHandler(const Message &message, Client *client);

	std::map<std::string, int (Server::*)(const Message &, Client *)>	replies;
	void																registerReplies(void);
	int																	eNoNickNameGivenHandler(const Message &message, Client *client);
	int																	eNeedMoreParamsHandler(const Message &message, Client *client);
	int																	eErroneusNickNameHandler(const Message &message, Client *client);
	int																	eNickNameInUseHandler(const Message &message, Client *client);
	int																	eAlreadyRegisteredHandler(const Message &message, Client *client);
	int																	ePassUnauthorizedHandler(const Message &message, Client *client);
	int																	eErroneusUserNameHandler(const Message &message, Client *client);
	int																	ePrefixHandler(const Message &message, Client *client);
	int																	rRegisterUserHandler(const Message &message, Client *client);
	int																	rWelcomeMessageHandler(const Message &message, Client *client);
	int																	rWelcomeHandler(const Message &message, Client *client);
	int																	rYourHostHandler(const Message &message, Client *client);
	int																	rCreatedHandler(const Message &message, Client *client);
	int																	rMyInfoHandler(const Message &message, Client *client);
	int																	rLUserClientHandler(const Message &message, Client *client);
	int																	rLUserChannelHandler(const Message &message, Client *client);
	int																	rLUserMeHandler(const Message &message, Client *client);
	int																	rMOTDHandler(const Message &message, Client *client);
	int																	rMOTDContentHandler(const Message &message, Client *client);
	int																	rMOTDStartHandler(const Message &message, Client *client);
	int																	rEndOfMOTDHandler(const Message &message, Client *client);
	int																	rNickBroadcastHandler(const Message &message, Client *client);
	int																	rUserBroadcastHandler(const Message &message, Client *client);
	int																	rPassHandler(const Message &message, Client *client);
	int																	rServerHandler(const Message &message, Client *client);
	int																	rHostHandler(const Message &message, Client *client);
	int																	rOtherServerHandler(const Message &message, Client *client);
	int																	rServerBroadcastHandler(const Message &message, Client *client);

	void																renewFd(const int fd);

	void																connectClient(void);
	void																disconnectClient(Client *client);

	void																receiveMessage(const int fd);
	void																sendMessage(const Message &message, Client *client);
	void																broadcastMessage(const Message &message, Client *client);
	void																sendAllInfo(Client *client);
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
};

#endif
