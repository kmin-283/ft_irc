/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmin <kmin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/07 14:09:56 by dakim             #+#    #+#             */
/*   Updated: 2020/12/07 14:42:17 by kmin             ###   ########.fr       */
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

int			Server::getSocket(void) const
{
	return (this->mainSocket);
}

void		Server::init(void)
{
	struct protoent 	*protocol;

	if (!(protocol = getprotobyname("tcp")))
		return ;
	this->mainSocket = socket(PF_UNSPEC, SOCK_STREAM, protocol->p_proto);
	if (this->mainSocket == -1)
		return ;

}

void		Server::listen(int port)
{
	std::cout << "Server is listen on " << port << std::endl;
}

void		Server::connectServer(std::string address)
{
	std::cout <<"addr = " << address << std::endl;
}
