#include "Server.hpp"

void	Server::registerCommands(void)
{
	this->commands["PASS"] = &Server::passHandler;
	this->commands["NICK"] = &Server::nickHandler;
	this->commands["USER"] = &Server::userHandler;
	this->commands["SERVER"] = &Server::serverHandler;
	this->commands["SQUIT"] = &Server::squitHandler;
	this->commands["WALLOPS"] = &Server::wallopsHandler;

	this->commands["VERSION"] = &Server::versionHandler;
	this->commands[RPL_VERSION] = &Server::versionHandler; // RPL로 어떻게 옮길까
	
	this->commands["STATS"] = &Server::statsHandler;
	this->commands[RPL_STATSCOMMANDS] = &Server::statsHandler;
	this->commands[RPL_ENDOFSTATS] = &Server::statsHandler;
}