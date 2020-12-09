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
	this->clearClient();
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

void			Server::init(const char *port)
{
	// int				flag;
	struct addrinfo	hints;
	struct addrinfo	*addrInfo;
	struct addrinfo	*addrInfoIterator;

	// flag = 1;
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

void			Server::start(void)
{
	while(42)
	{
		if (CONNECT_FAIL == select(this->maxFd + 1, &this->readFds, NULL, NULL, NULL))
			throw Server::SelectFailException();
		for (size_t listenFd = this->mainSocket; listenFd <= this->maxFd; ++listenFd)
		{
			if (FD_ISSET(listenFd, &this->readFds))
			{
				if (listenFd == this->mainSocket)
					this->acceptConnection();
				else
					this->receiveMessage(listenFd);
			}
		}
	}
}

void			Server::acceptConnection(void)
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


void			Server::receiveMessage(const size_t fd)
{
	char		buffer;
	int			readResult;
	std::string	message;
	Client 		*sender;

	message = "";
	readResult = 0;
	sender = this->acceptClients[fd];
	while (0 < (readResult = recv(sender->getFd(), &buffer, 1, 0)))
	{
		message += buffer;
		if (2 <= message.length() && "\r\n" == message.substr(message.length() - 2, message.length()))
		{
			// TODO 커멘트 파싱 필요
			// TODO 커멘드 처리하는 부분 필요
			message = "";
		}
	}
	if (readResult == -1 && errno != EAGAIN)
		throw Server::ReceiveMessageFailException();
	else if (readResult == 0)
	{
		close(sender->getFd());
		this->acceptClients.erase(sender->getFd());
		// TODO sendClients map에서 삭제할 필요 있음
		FD_CLR(sender->getFd(), &this->readFds);
		delete sender;
	}
}

struct addrinfo	*Server::getAddrInfo(std::string info)
{
	int				i;
	int				j;
	std::string		port;
	std::string		address;
	struct addrinfo	hints;
	struct addrinfo	*addrInfo;

	std::cout << "get addr info" << std::endl;

	i = info.rfind(":");
	j = info.rfind(":", i - 1);
	port = info.substr(j + 1, i - j - 1);
	address = info.substr(0, j);
	ft_memset(&hints, 0x00, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(address.c_str(), port.c_str(), &hints, &addrInfo) != 0)
		throw Server::GetAddressFailException();
	return (addrInfo);
}

void			Server::connectServer(std::string address)
{
	int					newFd;
	int					ipVersion;
	struct addrinfo		*addrInfo;
	struct addrinfo		*addrInfoIterator;

	ipVersion = AF_INET;
	addrInfo = this->getAddrInfo(address);
	for (addrInfoIterator = addrInfo; addrInfoIterator != NULL ; addrInfoIterator = addrInfoIterator->ai_next)
	{
		if (addrInfoIterator->ai_family == AF_INET6)
			ipVersion = AF_INET6;
	}
	for (addrInfoIterator = addrInfo; addrInfoIterator != NULL ; addrInfoIterator = addrInfoIterator->ai_next)
	{
		if (addrInfoIterator->ai_family == ipVersion)
		{
			if (CONNECT_FAIL == (newFd = socket(addrInfoIterator->ai_family, addrInfoIterator->ai_socktype, addrInfoIterator->ai_protocol)))
				throw Server::SocketOpenFailException();
			if (CONNECT_FAIL == connect(newFd, addrInfoIterator->ai_addr, addrInfoIterator->ai_addrlen))
			{
				close(newFd);
				std::cerr << ERROR_CONNECT_FAIL << std::endl;
			}
		}
	}
	freeaddrinfo(addrInfo);
	fcntl(newFd, F_SETFL, O_NONBLOCK);
	this->renewFd(newFd);
	Client *newClient = new Client(newFd);
	this->acceptClients.insert(std::pair<size_t, Client*>(newFd, newClient));

	// TODO 서버 등록관련 커멘드 전송 필요
	// PASS 123 /r/n SERVER 12312
	std::string password;

	password = address.substr(address.rfind(":") + 1, address.length() - 1);
	std::cout << "password = " << password << std::endl;
}

void			Server::clearClient(void)
{
	std::map<size_t, Client*>::iterator acceptIter = this->acceptClients.begin();
	std::map<std::string, Client*>::iterator senderIter = this->sendClients.begin();

	for (;acceptIter != this->acceptClients.end(); acceptIter++)
		delete acceptIter->second;
	for (;senderIter != this->sendClients.end(); senderIter++)
		delete senderIter->second;
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

const char		*Server::ReceiveMessageFailException::what() const throw()
{
	return ("ServerException:: Receive message fail");
}
