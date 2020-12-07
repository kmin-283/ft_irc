/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TLSServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmin <kmin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/07 14:10:31 by dakim             #+#    #+#             */
/*   Updated: 2020/12/07 17:47:48 by kmin             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TLSServer.hpp"

TLSServer::TLSServer(char *pass): Server(pass)
{
}

TLSServer::~TLSServer(void)
{
}

void		TLSServer::init(char *port)
{
	Server::init(port);
	/*tls설정*/
}

void		TLSServer::start(void)
{
}
