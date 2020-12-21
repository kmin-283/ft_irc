#include "Server.hpp"

void	Server::registerCommands(void)
{
	this->commands["PASS"] = &Server::passHandler;
	this->commands["NICK"] = &Server::nickHandler;
	this->commands["USER"] = &Server::userHandler;
	this->commands["SERVER"] = &Server::serverHandler;
}
