/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmin <kmin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/07 14:07:51 by dakim             #+#    #+#             */
/*   Updated: 2020/12/07 14:49:06 by kmin             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

# include <iostream>
# include <string>
# include <vector>
# include <list>
# include <queue>
# include <stack>
# include <map>
# include <algorithm>

# include <sys/socket.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/select.h>
# include <fcntl.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <stdlib.h>
# include <signal.h>
# include <unistd.h>
# include <limits.h>

# define SSL_PORT 6669

# define ERROR_ARG_NUM "Error: Too few or too few arguments."
# define ERROR_PORT "Error: Port must be numeric."

int				ft_atoi(char *str);
bool			ft_isdigit(char *str);

#endif
