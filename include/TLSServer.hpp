/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TLSServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmin <kmin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/07 14:07:24 by dakim             #+#    #+#             */
/*   Updated: 2020/12/07 14:41:55 by kmin             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TLS_SERVER_HPP
# define TLS_SERVER_HPP

# include "Server.hpp"

class				TLSServer : public Server
{
	public:
					TLSServer(char *pass);
	virtual			~TLSServer(void);
	virtual void	init(void);
	virtual void	listen(int port);
};

#endif
