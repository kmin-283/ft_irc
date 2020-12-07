/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmin <kmin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/07 14:04:30 by dakim             #+#    #+#             */
/*   Updated: 2020/12/07 17:48:00 by kmin             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "utils.hpp"

class				Server
{
private:
	std::string		pass;
	int				*mainSocket;
public:
					Server(const char *pass);
	virtual			~Server(void);

	virtual void	init(char *port);
	virtual void	start(void);
	void			connectServer(std::string address);

	std::string		getPass(void) const;
	int             *getSocket(void) const;

	class GetProtocolFailException: public std::exception
	{
		virtual const char* what() const throw();
	};
		class SocketOpenFailException: public std::exception
	{
		virtual const char* what() const throw();
	};
};

#endif
