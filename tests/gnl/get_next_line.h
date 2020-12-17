/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dakim <dakim@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/04 17:52:39 by dakim             #+#    #+#             */
/*   Updated: 2020/09/08 18:16:03 by dakim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <fcntl.h>
# include <stdlib.h>
# include <unistd.h>

typedef struct	s_g_list
{
	void			*content;
	struct s_g_list	*next;
}				t_g_list;
void			*ft_g_calloc(size_t nelem, size_t elsize);
int				ft_g_lstadd_back(t_g_list **lst, t_g_list *new);
t_g_list		*ft_g_lstnew(void *content);
t_g_list		*ft_g_lstlast(t_g_list *lst);
int				get_next_line(int fd, char **line);

#endif
