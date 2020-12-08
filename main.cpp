/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmin <kmin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/07 14:16:49 by dakim             #+#    #+#             */
/*   Updated: 2020/12/08 17:22:07 by kmin             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

int main(int argc, char *argv[])
{
	Server		*server;
	int			portIndex;
	int			passIndex;
	int			networkIndex;

	if (argc == 3 || argc == 4)
	{
		networkIndex = 1;
		portIndex = argc - 2;
		passIndex = argc - 1;
		if (ft_isdigit(argv[portIndex]))
		{
			if (std::string(argv[portIndex]) == std::string(SSL_PORT))
				server = new TLSServer(argv[passIndex]);
			else
				server = new Server(argv[passIndex]);
			server->init(argv[portIndex]);
			// if (argc == 4)
			// 	server->connectServer(argv[networkIndex]);
			// // TODO 달라질수도 있음
			server->start();
			delete server;
		}
		else
			std::cerr << ERROR_PORT << std::endl;
	}
	else
		std::cerr << ERROR_ARG_NUM << std::endl;
	return (0);
}
