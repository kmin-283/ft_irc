/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmin <kmin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/07 14:09:56 by dakim             #+#    #+#             */
/*   Updated: 2020/12/07 16:44:248 by kmin             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const char *pass)
	: pass(std::string(pass)), mainSocket(NULL)
{
}

Server::~Server(void)
{
}

std::string		Server::getPass(void) const
{
	return (this->pass);
}

int				*Server::getSocket(void) const
{
	return (this->mainSocket);
}
#include <string.h>

void		Server::init(char *port)
{
	int				i;
	int				flag;
	struct addrinfo	hints;
	struct addrinfo	*addrInfo;
	struct addrinfo	*addrInfoIterator;
	int	listenFdLen;

	listenFdLen = 0;
	memset(&hints, 0x00, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(NULL, port, &hints, &addrInfo) != 0)
		throw Server::GetProtocolFailException();
	for (addrInfoIterator = addrInfo; addrInfoIterator != NULL; addrInfoIterator = addrInfoIterator->ai_next)
		listenFdLen++;
	this->mainSocket = new int[listenFdLen];
	for (i = 0, addrInfoIterator = addrInfo; addrInfoIterator != NULL ; addrInfoIterator = addrInfoIterator->ai_next, i++)
	{
		flag = 1;
		if (SOCKET_FAIL == (this->mainSocket[i] = socket(addrInfoIterator->ai_family, addrInfoIterator->ai_socktype, addrInfoIterator->ai_protocol)))
			throw Server::SocketOpenFailException();
		// setsockopt(this->mainSocket[i], SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
		if (addrInfoIterator->ai_family == AF_INET6)
		{
			int opt = 1;
			setsockopt(this->mainSocket[i], IPPROTO_IPV6, IPV6_V6ONLY, (char *)&opt, sizeof(opt));
		}
		if (SOCKET_FAIL == (bind(this->mainSocket[i], addrInfoIterator->ai_addr, addrInfoIterator->ai_addrlen)))
			throw Server::SocketOpenFailException();
		if (SOCKET_FAIL == (listen(this->mainSocket[i], 42)))
			throw Server::SocketOpenFailException();

	}
	freeaddrinfo(addrInfo);
}

void		Server::start(void)
{
}

void		Server::connectServer(std::string address)
{
	std::cout <<"addr = " << address << std::endl;
}

const char		*Server::GetProtocolFailException::what() const throw()
{
	return ("BureaucratException: Grade too high");
}

const char		*Server::SocketOpenFailException::what() const throw()
{
	return ("BureaucratException: Grade too high");
}

