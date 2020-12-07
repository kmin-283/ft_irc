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

Server::Server(char *pass): pass(std::string(pass))
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

void		Server::init(char *port)
{
	int				i;
	int				flag;
	struct addrinfo	hints;
	struct addrinfo	*addrInfo;
	struct addrinfo	*addrInfoIterator;
	unsigned int	listenFdLen;

	flag = 1;
	listenFdLen = 0;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (!(getaddrinfo(0, port, &hints, &addrInfo)))
		throw Server::GetProtocolFailException();
	for (addrInfoIterator = addrInfo; addrInfoIterator != NULL; addrInfoIterator = addrInfoIterator->ai_next)
		listenFdLen++;
	this->mainSocket = new int[listenFdLen];
	for (i = 0, addrInfoIterator = addrInfo; addrInfoIterator != NULL ; addrInfoIterator = addrInfoIterator->ai_next, i++)
	{
		if (SOCKET_FAIL == (this->mainSocket[i] = socket(addrInfoIterator->ai_family, addrInfoIterator->ai_socktype, addrInfoIterator->ai_protocol)))
			throw Server::SocketOpenFailException();
		setsockopt(this->mainSocket[i], SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
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

