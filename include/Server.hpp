#ifndef SERVER_HPP
# define SERVER_HPP

# include "utils.hpp"
# include "Client.hpp"
# include "Channel.hpp"
# include "Message.hpp"
# include "Replies.hpp"

class																	Server
{
public:
	typedef std::map<int, Client>::iterator clientIter;
	typedef std::map<std::string, Client>::iterator strClientIter;
	typedef std::map<std::string, Client*>::iterator strClientPtrIter;
private:
	class Info
	{
	private:
		size_t															bytes;
		size_t															remoteCount;
		size_t															localCount;
		std::string														cmd;
	public:
																			Info();
																			~Info();
		void															incrementBytes(const int &size);
		std::string													getBytes() const;
		void															incrementRemoteCount(const int &size);
		std::string													getRemoteCount() const;
		void															incrementLocalCount(const int &size);
		std::string													getLocalCount() const;
	};
	std::vector<std::string>											cmd;

	std::string															prefix;

	std::string															ipAddress;
	std::string															serverName;
	std::string															version;
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

	bool																run;

	std::time_t															startTime;

	std::map<std::string, Info>											infos;

	std::map<std::string, int (Server::*)(const Message &, Client *)>	commands;
	void																registerCommands(void);
	int																	passHandler(const Message &message, Client *client);
	int																	setLocalNick(const Message &message, Client *client);
	int																	resetLocalNick(const Message &message, Client *client);
	int																	localNickHandler(const Message &message, Client *client);
	int																	setRemoteNick(const Message &message, Client *client);
	int																	resetRemoteNick(const Message &message, Client *client);
	int																	remoteNickHandler(const Message &message, Client *client);
	int																	nickHandler(const Message &message, Client *client);
	int																	setLocalUser(const Message &message, Client *client);
	int																	setRemoteUser(const Message &message, Client *client);
	int																	userHandler(const Message &message, Client *client);
	int																	localQuitHandler(const Message &message, Client *client);
	int																	remoteQuitHandler(const Message &message, Client *client);
	int																	quitHandler(const Message &message, Client *client);
	int																	serverHandler(const Message &message, Client *client);
	int																	squitHandler(const Message &message, Client *client);
	int																	wallopsHandler(const Message &message, Client *client);
	int																	versionHandler(const Message &message, Client *client);
	int																	statsHandler(const Message &message, Client *client);
	int																	linksHandler(const Message &message, Client *client);
	int																	timeHandler(const Message &message, Client *client);
	int																	connectHandler(const Message &message, Client *client);
	int																	privmsgHandler(const Message &message, Client *client);

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
	int																	eNickCollisionHandler(const Message &message, Client *client);
	int																	eCantKillServerHandler(const Message &message, Client *client);
	int																	eNoSuchServer(const Message &message, Client *client);
	int																	eUnknownCommand(const Message &message, Client *client);
	int																	eNoRecipients(const Message &message, Client *client);
	int																	eNoTextToSend(const Message &message, Client *client);


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
	int																	rNickHandler(const Message &message, Client *client);
	int																	rNickBroadcastHandler(const Message &message, Client *client);
	int																	rUserBroadcastHandler(const Message &message, Client *client);
	int																	rPassHandler(const Message &message, Client *client);
	int																	rServerHandler(const Message &message, Client *client);
	int																	rHostHandler(const Message &message, Client *client);
	int																	rOtherServerHandler(const Message &message, Client *client);
	int																	rServerBroadcastHandler(const Message &message, Client *client);
	int																	rKillHandler(const Message &message, Client *client);
	int																	rSquitBroadcastHandler(const Message &message, Client *client);
	int																	rQuitBroadcastHandler(const Message &message, Client *client);
	int																	rQuitHandler(const Message &message, Client *client);

	int																	rStatsL(const Message &message, Client *client);
	int																	rStatsM(const Message &message, Client *client);
	int																	rStatsO(const Message &message, Client *client);
	int																	rStatsU(const Message &message, Client *client);
	int																	rEndOfStats(const Message &message, Client *client);
	int																	rEndOfLinks(const Message &message, Client *client);

	void																renewFd(const int fd);

	void																getChildServer(std::list<std::string> &serverList, std::string key);
	std::string															getParentServer(std::string key);

	void																connectClient(void);
	void																disconnectClient(const Message &message, Client *client);
	void																disconnectChild(const Message &message, Client *client);
	void																clearClient(Client *client);
	void																deleteSubServers(const std::string &targetServer, const std::string &info);

	void																receiveMessage(const int fd);
	void																sendMessage(const Message &message, Client *client);
	void																broadcastMessage(const Message &message, Client *client);
	void																settingClient(const Message &message, Client *client);
	std::vector<std::string>											*getInfoFromWildcard(const std::string &info);

	void																initInfo(void);
	void																incrementLcountAndByte(const std::string &command, const Message &message);
	void																incrementRcountAndByte(const std::string &command, const Message &message);

	Client																*hasTarget(const std::string &target, strClientPtrIter start, strClientPtrIter end);
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
