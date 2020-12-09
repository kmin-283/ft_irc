/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TLSServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmin <kmin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/07 14:10:31 by dakim             #+#    #+#             */
/*   Updated: 2020/12/08 17:33:27 by kmin             ###   ########.fr       */
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
	Server::init(port);
	/*tls설정*/
}

void		TLSServer::start(void)
{
}
