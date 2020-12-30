#include "Server.hpp"

void	Server::registerCommands(void)
{
	this->commands["PASS"] = &Server::passHandler;
	this->commands["NICK"] = &Server::nickHandler;
	this->commands["USER"] = &Server::userHandler;
	this->commands["SERVER"] = &Server::serverHandler;
	this->commands["SQUIT"] = &Server::squitHandler;
	this->commands["WALLOPS"] = &Server::wallopsHandler;
	this->commands["version"] = &Server::versionHandler;
	this->commands["VERSION"] = &Server::versionHandler;
	this->commands["351"] = &Server::versionHandler;
}