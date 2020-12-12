#include "TLSServer.hpp"

TLSServer::TLSServer(const char *pass): Server(pass)
{
}

TLSServer::~TLSServer(void)
{
}

void		TLSServer::init(const char *port)
{
	std::cout << "TLS server" << std::endl;
	Server::init(port);
	/*tls설정*/
}

void		TLSServer::acceptConnection(void)
{
	std::cout << "TLS server" << std::endl;
	Server::acceptConnection();
}

void		TLSServer::receiveMessage(const int fd)
{
	std::cout << "TLS server" << std::endl;
	Server::receiveMessage(fd);
}
