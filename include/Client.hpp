/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmin <kmin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/08 17:02:52 by dakim             #+#    #+#             */
/*   Updated: 2020/12/09 13:10:10 by kmin             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "utils.hpp"

class				Client
{
private:
	size_t			fd;
public:
					Client(const size_t fd);
					~Client(void);
	int				getFd(void) const;
};

#endif
