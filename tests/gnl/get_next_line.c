/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dakim <dakim@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/12 15:50:36 by dakim             #+#    #+#             */
/*   Updated: 2020/09/08 18:34:39 by dakim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static int	ft_free_alloc(t_g_list **lst, int return_value)
{
	t_g_list *tmp_node;

	if (*lst)
	{
		while (*lst)
		{
			tmp_node = NULL;
			tmp_node = *lst;
			*lst = (*lst)->next;
			free(tmp_node->content);
			free(tmp_node);
		}
		*lst = NULL;
	}
	return (return_value);
}

static int	ft_count_len(t_g_list *lst, int size)
{
	int		i;
	int		count;

	count = 0;
	while (lst)
	{
		i = -1;
		while (++i < size)
		{
			if (*((char*)lst->content + i) == '\n')
				return (count);
			++count;
		}
		lst = lst->next;
	}
	return (count);
}

static int	ft_lst_to_line(t_g_list **lst, char **line, int size, int line_size)
{
	t_g_list	*tmp_lst;
	int			i;
	int			j;

	j = -1;
	while (*lst)
	{
		i = -1;
		tmp_lst = *lst;
		while (++i < size && j < line_size)
		{
			if (*((char*)(*lst)->content + i) == '\n')
			{
				*((char*)(*lst)->content + i) = 0;
				return (0);
			}
			if (*((char*)(*lst)->content + i))
				*(*line + ++j) = *((char*)(*lst)->content + i);
			*((char*)(*lst)->content + i) = 0;
		}
		*lst = (*lst)->next;
		tmp_lst->next = NULL;
		ft_free_alloc(&tmp_lst, 0);
	}
	return (1);
}

static int	ft_check_newline(char *str, int size)
{
	int i;

	i = -1;
	while (++i < size)
	{
		if (*(str + i) == '\n')
			return (1);
	}
	return (0);
}

int			get_next_line(int fd, char **line)
{
	static t_g_list	*lst[1024];
	int				read_result;
	int				size;

	if ((size = BUFFER_SIZE) <= 0 || fd < 0 || !line)
		return (-1);
	if (ft_g_lstadd_back(&lst[fd],
				ft_g_lstnew(ft_g_calloc(sizeof(char), size + 1))))
		return (ft_free_alloc(&lst[fd], -1));
	while (0 < (read_result =
				read(fd, ft_g_lstlast(lst[fd])->content, BUFFER_SIZE)))
	{
		if (ft_check_newline(ft_g_lstlast(lst[fd])->content, size))
			break ;
		if (ft_g_lstadd_back(&lst[fd],
					ft_g_lstnew(ft_g_calloc(sizeof(char), size + 1))))
			return (ft_free_alloc(&lst[fd], -1));
	}
	if (read_result == -1)
		return (ft_free_alloc(&lst[fd], -1));
	if (!(*line = ft_g_calloc(sizeof(char), (ft_count_len(lst[fd], size) + 1))))
		return (ft_free_alloc(&lst[fd], -1));
	if (ft_lst_to_line(&lst[fd], line, size, ft_count_len(lst[fd], size)))
		return (ft_free_alloc(&lst[fd], 0));
	return (1);
}
