/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmin <kmin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/07 14:04:30 by dakim             #+#    #+#             */
/*   Updated: 2020/12/09 13:03:45 by kmin             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "utils.hpp"
# include "Client.hpp"

class				Server
{
private:
	std::string						pass;
	size_t							mainSocket;
	size_t							maxFd;
	fd_set							readFds;
	std::map<size_t, Client*>		acceptClients;
	std::map<std::string, Client*>	sendClients;
public:
					Server(const char *pass);
	virtual			~Server(void);

	virtual void	init(const char *port);
	virtual void	start(void);
	void			connectServer(const std::string address);
	void			acceptConnection(void);
	void			receiveMessage(const size_t fd);

	std::string		getPass(void) const;
	int				getSocket(void) const;
	void			renewFd(const size_t fd);

	class GetAddressFailException: public std::exception
	{
		virtual const char* what() const throw();
	};
	class SocketOpenFailException: public std::exception
	{
		virtual const char* what() const throw();
	};
	class SocketBindFailException: public std::exception
	{
		virtual const char* what() const throw();
	};
	class SocketListenFailException: public std::exception
	{
		virtual const char* what() const throw();
	};
	class SelectFailException: public std::exception
	{
		virtual const char* what() const throw();
	};
	class AcceptFailException: public std::exception
	{
		virtual const char* what() const throw();
	};
	class ReceiveMessageFailException: public std::exception
	{
		virtual const char* what() const throw();
	};
};

#endif
