/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: egaliber <egaliber@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/24 15:34:07 by elliotgalib       #+#    #+#             */
/*   Updated: 2022/01/28 17:06:31 by egaliber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static int	gnl_verify_line(char **stack, char **line)
{
	char *temp;
	int i;

	i = 0;
	while ((*stack)[i] != '\n' && (*stack)[i] != '\0')
		i++;
	if ((*stack)[i] == '\n')
	{
		*line = ft_strsub(*stack, 0, i);
		temp = ft_strdup(&(*stack)[i + 1]);
		free(*stack);
		*stack = temp;
		if ((*stack)[0] == '\0')
			ft_strdel(stack);
		return (1);
	}
	else
	{
		*line = ft_strdup(*stack);
		ft_strdel(stack);
		return (1);
	}
}

static int	ret_value(char **stack, char **line, int ret, int fd)
{
	if (ret < 0)
		return (-1);
	else if (ret == 0 && stack[fd] == NULL)
	{
		*line = NULL;
		return (0);
	}
	else
		return (gnl_verify_line(&stack[fd], line));
}

static	int	gnl_read_file(int fd, char *heap, char **stack)
{
	int				ret;
	char			*tmp_stack;

	ret = read(fd, heap, BUFF_SIZE);
	while (ret > 0)
	{
		heap[ret] = '\0';
		if (stack[fd])
		{
			tmp_stack = ft_strjoin(stack[fd], heap);
			if(!tmp_stack)
				return(-1);
			free (stack[fd]);
			stack[fd] = tmp_stack;
		}
		else
			stack[fd] = ft_strdup(heap);
		if (ft_strchr(stack[fd], '\n'))
			return (ret);
		ret = read(fd, heap, BUFF_SIZE);
	}
	return (ret);
}

int	get_next_line(int const fd, char **line)
{
	static char	*stack[MAX_FD];
	char		heap[BUFF_SIZE + 1];
	int			ret;

	if (!line || fd < 0 || fd >= MAX_FD)
		return (-1);
	ret = gnl_read_file(fd, heap, stack);
	return (ret_value(stack, line, ret, fd));
}
