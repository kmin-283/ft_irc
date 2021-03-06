#ifndef SERVER_HPP
# define SERVER_HPP

# include "utils.hpp"
# include "Client.hpp"
# include "Channel.hpp"
# include "Message.hpp"
# include "Replies.hpp"
# include <openssl/ssl.h>
# include <openssl/err.h>

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

//	std::map<std::string, Channel>                                      localChannel;
//    std::map<std::string, Channel>                                      remoteChannel;
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
	std::string 														tlsPort;
	int																	mainSocket;
	int 																tlsMainSocket;
	int																	maxFd;

	fd_set																readFds;

	std::map<int, Client>												acceptClients;
	std::map<std::string, Client>										sendClients;
	std::map<std::string, Client *>										serverList;
	std::map<std::string, Client *>										clientList;
	std::map<std::string, Client *>										serviceList;
	std::map<std::string, Channel>										localChannelList;
	std::map<std::string, Channel>										remoteChannelList;

	bool																run;

	std::string                                                         adminLoc1;
	std::string                                                         adminLoc2;
	std::string                                                         adminEmail;
	std::time_t															startTime;

	std::map<std::string, Info>											infosPerCommand;

	time_t                                                              pingTime;
	bool                                                                isDeletedClient;
	SSL_CTX																*ctx;
	SSL																	*ssl;
	bool																isSSL;
	int																	sslFd;


	SSL_CTX																*InitCTX(void);
	void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile, bool serv);

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

	int																	traceHandler(const Message &message, Client *client);
	int																	privmsgHandler(const Message &message, Client *client);
	int 																noticeHandler(const Message &message, Client *client);

	int     															joinHandler(const Message &message, Client *client);
	int     															partHandler(const Message &message, Client *client);
	int     															topicHandler(const Message &message, Client *client);
	int                                                                 modeHandler(const Message &message, Client *client);
	int 																modeHelper(std::string &error, size_t &modeIndex, const Message &message, Client *client, Client *exceptClient);
	void																showBanList(Channel &channel, Client *client);
	void																showExceptionList(Channel &channel, Client *client);
	void 																showInvitationList(Channel &channel, Client *client);

	int     															namesHandler(const Message &message, Client *client);
	int     															listHandler(const Message &message, Client *client);
	int     															inviteHandler(const Message &message, Client *client);
	int         														kickHandler(const Message &message, Client *client);

	int                                                                 adminHandler(const Message &message, Client *client);

	int                                                                 infoHandler(const Message &message, Client *client);

	int                                                                 pingHandler(const Message &message, Client *client);
  	int                                                                 pongHandler(const Message &message, Client *client);

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
	int																	eNotRegistered(const Message &message, Client *client);
	int																	eNoRecipients(const Message &message, Client *client);
	int																	eNoTextToSend(const Message &message, Client *client);
	int																	eNoSuchNick(const Message &message, Client *client);
	int                                                                 eNoOrigin(const Message &message, Client *client);
	int 																eUnknownMode(const Message &message, Client *client);
	int																	eUserNotInChannel(const Message &message, Client *client);
	int 																eBadChannelKey(const Message &message, Client *client);
	int																	eInviteOnlyChan(const Message &message, Client *client);
	int																	eBannedFromChan(const Message &message, Client *client);

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
	int																	rUserModeBroadcastHandler(const Message &message, Client *client);
	int																	rPassHandler(const Message &message, Client *client);
	int																	rServerHandler(const Message &message, Client *client);
	int																	rHostHandler(const Message &message, Client *client);
	int																	rOtherServerHandler(const Message &message, Client *client);
	int																	rServerBroadcastHandler(const Message &message, Client *client);
	int																	rKillHandler(const Message &message, Client *client);
	int																	rSquitBroadcastHandler(const Message &message, Client *client);
	int																	rQuitBroadcastHandler(const Message &message, Client *client);
	int																	rQuitHandler(const Message &message, Client *client);
	int																	rInviting(const Message &message, Client *client);

	int																	rReplyHandler(const Message &message, Client *client);

	int																	rStatsL(const Message &message, Client *client);
	int																	rStatsM(const Message &message, Client *client);
	int																	rStatsO(const Message &message, Client *client);
	int																	rStatsU(const Message &message, Client *client);
	int																	rEndOfStats(const Message &message, Client *client);

	int																	rEndOfLinks(const Message &message, Client *client);

	void																renewFd(const int fd);

	void																getChildServer(std::list<std::string> &serverList, std::string key);
	std::string															getParentServer(std::string key);

	void																connectClient(const int &listenFd);
	void																disconnectClient(const Message &message, Client *client);
	void																disconnectChild(const Message &message, Client *client);
	void																clearClient(void);
	void																deleteSubServers(const std::string &targetServer, const std::string &info);

	void																receiveMessage(const int &fd);
	void																sendMessage(const Message &message, Client *client);
	void																broadcastMessage(const Message &message, Client *client);
	void																settingClient(const Message &message, Client *client);
	std::vector<std::string>											*getInfoFromWildcard(const std::string &info);

	void																initInfosPerCommand(void);
	void																incrementLocalByte(Client *client, const Message &message);
	void																incrementRemoteByte(Client *client, const Message &message);

	Client																*hasTarget(const std::string &target, strClientPtrIter start, strClientPtrIter end);

	// privmsgHandler에 있음 join도 사용
	std::string     													getClientPrefix(Client *client);

	void 																sendChannelLists(Client *client);

	// 디버깅용!
	int																	show(const Message &message, Client *client);
public:
																		Server(const char *pass, const char *port);
																		~Server(void);
	void																init(const char *port);
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
