/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmin <kmin@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/07 14:15:12 by dakim             #+#    #+#             */
/*   Updated: 2020/12/08 13:46:25 by kmin             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

static int		ft_check_range(unsigned long long number, int sign)
{
	if (number > LLONG_MAX - 1 && sign == -1)
		return (0);
	if (number > LLONG_MAX && sign == 1)
		return (-1);
	return (number * sign);
}

int				ft_atoi(char *str)
{
	unsigned long long	number_value;
	int					sign_value;
	int					i;

	i = 0;
	number_value = 0;
	sign_value = 1;
	while ((*(str + i) == '\t' || *(str + i) == '\n'
		|| *(str + i) == '\v' || *(str + i) == '\f'
		|| *(str + i) == '\r' || *(str + i) == ' ') && *(str + i))
		i++;
	if (*(str + i) == '-')
		sign_value = -1;
	if (*(str + i) == '-' || *(str + i) == '+')
		i++;
	while (('0' <= *(str + i) && *(str + i) <= '9') && *(str + i))
	{
		number_value = number_value * 10 + (*(str + i) - '0');
		i++;
	}
	return (ft_check_range(number_value, sign_value));
}

bool			ft_isdigit(char *str)
{
	int			i;

	i = -1;
	while (str[++i])
	{
		if (!('0' <= str[i] && str[i] <= '9'))
			return (false);
	}
	return (true);
}

void			*ft_memset(void *s, int c, size_t n)
{
		size_t	i;
		char	*str;

		str = (char *)s;
		i = -1;
		while (++i < n)
			*(str + i) = c;
		s = str;
		return (s);
}