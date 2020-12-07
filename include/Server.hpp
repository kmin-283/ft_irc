/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmin <kmin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/07 14:04:30 by dakim             #+#    #+#             */
/*   Updated: 2020/12/07 14:42:09 by kmin             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "utils.hpp"

class				Server
{
	private:
	std::string		pass;
	int				mainSocket;
	public:
					Server(char *pass);
	virtual			~Server(void);

	virtual void	init(void);
	virtual void	listen(int port);

	void			connectServer(std::string address);

	std::string		getPass(void) const;
	int				getSocket(void) const;
};

#endif
