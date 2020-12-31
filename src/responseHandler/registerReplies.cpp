#include "Server.hpp"

void			Server::registerReplies(void)
{
	this->replies[ERR_NONICKNAMEGIVEN] = &Server::eNoNickNameGivenHandler;
	this->replies[ERR_NEEDMOREPARAMS] = &Server::eNeedMoreParamsHandler;
	this->replies[ERR_ERRONEUSNICKNAME] = &Server::eErroneusNickNameHandler;
	this->replies[ERR_NICKNAMEINUSE] = &Server::eNickNameInUseHandler;
	this->replies[ERR_ALREADYREGISTRED] = &Server::eAlreadyRegisteredHandler;
	this->replies[ERR_PASSUNAUTHORIE] = &Server::ePassUnauthorizedHandler;
	this->replies[ERR_ERRONEUSUSERNAME] = &Server::eErroneusUserNameHandler;
	this->replies[ERR_PREFIX] = &Server::ePrefixHandler;
	this->replies[ERR_NICKCOLLISION] = &Server::eNickCollisionHandler;
	this->replies[ERR_CANTKILLSERVER] = &Server::eCantKillServerHandler;

	this->replies[RPL_REGISTERUSER] = &Server::rRegisterUserHandler;
	this->replies[RPL_WELCOMEMESSAGE] = &Server::rWelcomeMessageHandler;
	this->replies[RPL_WELCOME] = &Server::rWelcomeHandler;
	this->replies[RPL_YOURHOST] = &Server::rYourHostHandler;
	this->replies[RPL_CREATED] = &Server::rCreatedHandler;
	this->replies[RPL_MYINFO] = &Server::rMyInfoHandler;
	this->replies[RPL_LUSERCLIENT] = &Server::rLUserClientHandler;
	this->replies[RPL_LUSERCHANNELS] = &Server::rLUserChannelHandler;
	this->replies[RPL_LUSERME] = &Server::rLUserMeHandler;
	this->replies[RPL_MOTD] = &Server::rMOTDHandler;
	this->replies[RPL_MOTDCONTENT] = &Server::rMOTDContentHandler;
	this->replies[RPL_MOTDSTART] = &Server::rMOTDStartHandler;
	this->replies[RPL_ENDOFMOTD] = &Server::rEndOfMOTDHandler;
	this->replies[RPL_NICK] = &Server::rNickHandler;
	this->replies[RPL_NICKBROADCAST] = &Server::rNickBroadcastHandler;
	this->replies[RPL_USERBROADCAST] = &Server::rUserBroadcastHandler;
	this->replies[RPL_PASS] = &Server::rPassHandler;
	this->replies[RPL_SERVER] = &Server::rServerHandler;
	this->replies[RPL_HOST] = &Server::rHostHandler;
	this->replies[RPL_OTHERSERVER] = &Server::rOtherServerHandler;
	this->replies[RPL_SERVERBROADCAST] = &Server::rServerBroadcastHandler;
	this->replies[RPL_KILL] = &Server::rKillHandler;
	this->replies[RPL_SQUITBROADCAST] = &Server::rSquitBroadcastHandler;
	this->replies[RPL_QUITBROADCAST] = &Server::rQuitBroadcastHandler;

	// this->replies[RPL_STATSCOMMANDS] = &Server::rStatsHandler;
	// this->replies[RPL_ENDOFSTATS] = &Server::rEndOfStats;
	this->replies[STATS_M] = &Server::rStatsM;
}
