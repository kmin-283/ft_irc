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
	this->commands[RPL_VERSION] = &Server::versionHandler;
	
	this->commands["STATS"] = &Server::statsHandler;
	this->commands[RPL_STATSLINKINFO] = &Server::statsHandler;
	this->commands[RPL_STATSCOMMANDS] = &Server::statsHandler;
	this->commands[RPL_STATSUPTIME] = &Server::statsHandler;
	this->commands[RPL_ENDOFSTATS] = &Server::statsHandler;

	this->commands["LINKS"] = &Server::linksHandler;
	this->commands[RPL_LINKS] = &Server::linksHandler;
	this->commands[RPL_ENDOFLINKS] = &Server::linksHandler;

	this->commands["TIME"] = &Server::timeHandler;
	this->commands[RPL_TIME] = &Server::timeHandler;

	this->commands["CONNECT"] = &Server::connectHandler;
}
