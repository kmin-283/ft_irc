/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TLSServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmin <kmin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/07 14:10:31 by dakim             #+#    #+#             */
/*   Updated: 2020/12/09 16:53:59 by kmin             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

void		TLSServer::receiveMessage(const size_t fd)
{
	std::cout << "TLS server" << std::endl;
	Server::receiveMessage(fd);
}
