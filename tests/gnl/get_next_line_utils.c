/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dakim <dakim@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/12 16:09:45 by dakim             #+#    #+#             */
/*   Updated: 2020/06/07 17:37:48 by dakim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static void	*ft_memset(void *s, int c, size_t n)
{
	size_t	i;
	char	*str;

	str = s;
	i = -1;
	while (++i < n)
	{
		*(str + i) = c;
	}
	s = str;
	return (s);
}

void		*ft_g_calloc(size_t nelem, size_t elsize)
{
	void	*return_str;

	if (!(return_str = malloc(nelem * elsize)))
		return (NULL);
	ft_memset(return_str, 0, nelem * elsize);
	return (return_str);
}

t_g_list	*ft_g_lstnew(void *content)
{
	t_g_list *return_node;

	return_node = NULL;
	if (content)
	{
		if ((return_node = (t_g_list *)malloc(sizeof(t_g_list))))
		{
			return_node->next = NULL;
			return_node->content = content;
		}
		else
			free(content);
	}
	return (return_node);
}

t_g_list	*ft_g_lstlast(t_g_list *lst)
{
	if (lst)
	{
		while (lst->next)
			lst = lst->next;
	}
	return (lst);
}

int			ft_g_lstadd_back(t_g_list **lst, t_g_list *new)
{
	if (new && lst)
	{
		if (ft_g_lstlast(*lst))
			ft_g_lstlast(*lst)->next = new;
		else
			(*lst) = new;
		return (0);
	}
	else if (new)
		free(new);
	return (1);
}
