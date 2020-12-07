/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TLSServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmin <kmin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/07 14:10:31 by dakim             #+#    #+#             */
/*   Updated: 2020/12/07 14:41:45 by kmin             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TLSServer.hpp"

TLSServer::TLSServer(char *pass): Server(pass)
{
}

TLSServer::~TLSServer(void)
{
}

void		TLSServer::init(void)
{
	std::cout << "TLS Server init" << std::endl;
	std::cout << "pass = " << this->getPass() << std::endl;
}

void		TLSServer::listen(int port)
{
	std::cout << "TLS Server is listen on " << port << std::endl;
}
