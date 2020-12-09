/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TLSServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmin <kmin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/07 14:07:24 by dakim             #+#    #+#             */
/*   Updated: 2020/12/09 16:44:53 by kmin             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TLS_SERVER_HPP
# define TLS_SERVER_HPP

# include "Server.hpp"

class				TLSServer : public Server
{
	public:
					TLSServer(const char *pass);
	virtual			~TLSServer(void);
	virtual void	init(const char *port);
	virtual void	acceptConnection(void);
	virtual void	receiveMessage(const size_t fd);


};

#endif
