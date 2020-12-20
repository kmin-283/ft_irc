#include "Server.hpp"

void			Server::registerReplies(void)
{
	this->replies[ERR_NONICKNAMEGIVEN] = &Server::eNoNickNameGivenHandler;
	this->replies[ERR_NEEDMOREPARAMS] = &Server::eNeedMoreParamsHandler;
	this->replies[ERR_ERRONEUSNICKNAME] = &Server::eErroneusNickNameHandler;
	this->replies[ERR_NICKNAMEINUSE] = &Server::eNickNameInUseHandler;


	this->replies[RPL_REGISTER_USER] = &Server::rRegisterUserHandler;
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
}
