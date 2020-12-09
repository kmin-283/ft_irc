/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmin <kmin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/08 17:05:37 by dakim             #+#    #+#             */
/*   Updated: 2020/12/09 13:10:13 by kmin             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(const size_t fd)
	: fd(fd)
{
}

Client::~Client(void)
{
}

int				Client::getFd(void) const
{
	return (this->fd);
}
