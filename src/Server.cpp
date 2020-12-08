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
	: pass(std::string(pass)), mainSocket(0)
{
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
			if (SOCKET_FAIL == (this->mainSocket = socket(addrInfoIterator->ai_family, addrInfoIterator->ai_socktype, addrInfoIterator->ai_protocol)))
				throw Server::SocketOpenFailException();
			setsockopt(this->mainSocket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
			if (SOCKET_FAIL == (bind(this->mainSocket, addrInfoIterator->ai_addr, addrInfoIterator->ai_addrlen)))
				throw Server::SocketBindFailException();
			if (SOCKET_FAIL == (listen(this->mainSocket, SOMAXCONN)))
				throw Server::SocketListenFailException();
		}
	}
	freeaddrinfo(addrInfo);
}

void		Server::start(void)
{
	int listenFd;
	int fdNum;
	int maxfd = 0;
	int readn;
	fd_set readfds, allfds;

	// while(42)
	// {
	// 	serv_select();
	// 	for (int i = 3; i <= fdmax; ++i)
	// 	{
	// 		// 파일디스크립터에 변화가 생긴경우
	// 		if (FD_ISSET(i, &read_fds))
	// 		{
	// 			// listener의 경우 처음 소켓을 열었을 때 얻은 파일디스크립터임
	// 			// getIP함수에 socket으로부터 파일디스트립터를 listener에 저장하는 부분 있음
	// 			if (i == listener)
	// 			{
	// 				// listener의 fd에 변경이 있는경우
	// 				// => 새로 연결하는 경우
	// 				new_connection();
	// 			}
	// 			else
	// 			{
	// 				// listener가 아닌 다른 fd에 변경이 있는경우
	// 				// => 새로운 연결이 아닌경우
	// 				receive(i);
	// 			}
	// 		}
	// 	}
	// }
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