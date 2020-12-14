#include "TLSServer.hpp"

TLSServer::TLSServer(const char *pass, const char *port): Server(pass, port)
{
}

TLSServer::~TLSServer(void)
{
}

void		TLSServer::init(void)
{
	std::cout << "TLS server" << std::endl;
	Server::init();
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
