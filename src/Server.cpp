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
	: pass(std::string(pass)), mainSocket(0), maxFd(0)
{
	FD_ZERO(&this->readFds);
}

Server::~Server(void)
{
}

std::string		Server::getPass(void) const
{
	return (this->pass);
}

int				Server::getSocket(void) const
{
	return (this->mainSocket);
}

void			Server::renewFd(const size_t fd)
{
	FD_SET(fd, &this->readFds);
	if (this->maxFd < fd)
		this->maxFd = fd;
}

void		Server::init(char *port)
{
	int				flag;
	struct addrinfo	hints;
	struct addrinfo	*addrInfo;
	struct addrinfo	*addrInfoIterator;

	flag = 1;
	ft_memset(&hints, 0x00, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(NULL, port, &hints, &addrInfo) != 0)
		throw Server::GetAddressFailException();
	for (addrInfoIterator = addrInfo; addrInfoIterator != NULL ; addrInfoIterator = addrInfoIterator->ai_next)
	{
		if (addrInfoIterator->ai_family == AF_INET6)
		{
			if (CONNECT_FAIL == (this->mainSocket = socket(addrInfoIterator->ai_family, addrInfoIterator->ai_socktype, addrInfoIterator->ai_protocol)))
				throw Server::SocketOpenFailException();
			// setsockopt(this->mainSocket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
			if (CONNECT_FAIL == (bind(this->mainSocket, addrInfoIterator->ai_addr, addrInfoIterator->ai_addrlen)))
				throw Server::SocketBindFailException();
			if (CONNECT_FAIL == (listen(this->mainSocket, SOMAXCONN)))
				throw Server::SocketListenFailException();
		}
	}
	this->renewFd(this->mainSocket);
	freeaddrinfo(addrInfo);
}

void		Server::start(void)
{
	// while(42)
	// {
		if (CONNECT_FAIL == select(this->maxFd + 1, &this->readFds, NULL, NULL, NULL))
			throw Server::SelectFailException();
		for (size_t listenFd = this->mainSocket; listenFd <= this->maxFd; ++listenFd)
		{
			if (FD_ISSET(listenFd, &this->readFds))
			{
				if (listenFd == this->mainSocket)
					this->acceptConnection();
				// else
					// this->receiveMessage(listenFd);
			}
		}
	// }
}

void		Server::acceptConnection(void)
{
	int					newFd;
	struct sockaddr_in6	remoteAddress;
	socklen_t			addressLen;

	addressLen = sizeof(struct sockaddr_in);
	if (CONNECT_FAIL == (newFd = accept(this->mainSocket, (struct sockaddr*)&remoteAddress, &addressLen)))
		throw Server::AcceptFailException();
	fcntl(newFd, F_SETFL, O_NONBLOCK);
	this->renewFd(newFd);
	Client *newClient = new Client(newFd);
	this->acceptClients.insert(std::pair<size_t, Client*>(newFd, newClient));
}

void		Server::connectServer(std::string address)
{
	std::cout <<"addr = " << address << std::endl;
}

const char		*Server::GetAddressFailException::what() const throw()
{
	return ("ServerException:: Get address info fail");
}

const char		*Server::SocketOpenFailException::what() const throw()
{
	return ("ServerException:: Socket open fail");
}

const char		*Server::SocketBindFailException::what() const throw()
{
	return ("ServerException:: Socket bind fail");
}

const char		*Server::SocketListenFailException::what() const throw()
{
	return ("ServerException:: Socket listen fail");
}

const char		*Server::SelectFailException::what() const throw()
{
	return ("ServerException:: Select fail");
}

const char		*Server::AcceptFailException::what() const throw()
{
	return ("ServerException:: Accept fail");
}