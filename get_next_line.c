/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elliotgalibert <elliotgalibert@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/24 15:34:07 by elliotgalib       #+#    #+#             */
/*   Updated: 2022/01/24 15:49:52 by elliotgalib      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

/*
** Allokoidaan tarpeeksi muistia jotta s1 string voidaan kopioida.
** Tekee kopion ja paluttaa pointterin siihen.
*/

static char			*ft_strdup(const char *s1)
{
	char		*s2;
	size_t		i;

	i = 0;
	while (s1[i])
		i += 1;
	if (!(s2 = (char *)malloc(sizeof(char) * (i + 1))))
		return (NULL);
	i = -1;
	while (s1[++i])
		s2[i] = s1[i];
	s2[i] = '\0';
	return (s2);
}

/*
** Allokoidaan mallocilla ja palautta fresh stringin joka loppuu '\0' seon s1 ja s2 ketjutuksen tulos.
** jos Fail palautetaan NULL.
*/

static char			*ft_strjoin(char const *s1, char const *s2)
{
	char		*s3;
	char		*tmp_s3;
	size_t		i;
	size_t		j;

	j = 0;
	i = 0;
	while (s1[i])
		i += 1;
	while (s2[j])
		j += 1;
	if (!s1 || !s2 || !(s3 = (char *)malloc(sizeof(char) * (i + j + 1))))
		return (NULL);
	tmp_s3 = s3;
	while (*s1 != '\0')
		*tmp_s3++ = *s1++;
	while (*s2 != '\0')
		*tmp_s3++ = *s2++;
	*tmp_s3 = '\0';
	return (s3);
}

/*
** Tarkastetaan mitä onko stackissä olevalla "tavaralla" newlineä. Jos ei palauta 0.
** Jos on newline tehdään kopio stackistä (on the line) ja 
** Myös kaikki mitä stackissä oli kopioitiin, joten se palautetaan stackiin. (Temp stack)
** Verify if whatever is in the stack, has a newline. If it doesn't, returns
*/

static int			gnl_verify_line(char **stack, char **line)
{
	char			*tmp_stack;
	char			*strchr_stack;
	int				i;

	i = 0;
	strchr_stack = *stack;
	while (strchr_stack[i] != '\n')
		if (!strchr_stack[i++])
			return (0);
	tmp_stack = &strchr_stack[i];
	*tmp_stack = '\0';
	*line = ft_strdup(*stack);
	*stack = ft_strdup(tmp_stack + 1);
	return (1);
}

/*
** Lukee heappiin Fd:stä tietyn verran bittejä eli BUFF_SIZE, joka on valittu .h filessä.
** Jatkaa lukemista siihen asti, kunnes palautus read functionissa on isompi, kuin 0. (ei ongelmia)
** Jos stackissa on jotain, ketjutetaan kaikki, mitä siellä on ja mitä heap lukee. Jos ei lisätään heap stackiin.
** Sitten tarkistetaan stack, newlinen varalta. 
** Jos on newline tullaan ulos while loopista ja pakotetaan positiivinen ret eli 1. RET_VALUE:n avulla.
*/

static	int			gnl_read_file(int fd, char *heap, char **stack, char **line)
{
	int				ret;
	char			*tmp_stack;

	while ((ret = read(fd, heap, BUFF_SIZE)) > 0)
	{
		heap[ret] = '\0';
		if (*stack)
		{
			tmp_stack = *stack;
			*stack = ft_strjoin(tmp_stack, heap);
			free(tmp_stack);
			tmp_stack = NULL;
		}
		else
			*stack = ft_strdup(heap);
		if (gnl_verify_line(stack, line))
			break ;
	}
	return (RET_VALUE(ret));
}

/*
** Ensin tarkastetaan virheet, onko FD:n osoittaja virheellinen tai
** jos kusee se palauttaa -1 jos tarvitsee.
**
** Jos tekstistä löytyy jotain tarkastetaan, että tulee newline, jos ei allocoidaan muistia
** ja jatketaan filen lukemista
**
** Kun filen lukeminen loppuu vapautetaan heap, koska sitä ei tarvita enään
** Katsotaan myös palauttaako se 1 vai -1, mutta jos stack on tyhjä ret 0.
** Jos mikään näistä ei tule toteen palutetaan stack ja ret 1.
*/

int					get_next_line(int const fd, char **line)
{
	static char		*stack[MAX_FD];
	char			*heap;
	int				ret;
	int				i;

	if (!line || (fd < 0 || fd >= MAX_FD) || (read(fd, stack[fd], 0) < 0) \
		|| !(heap = (char *)malloc(sizeof(char) * BUFF_SIZE + 1)))
		return (-1);
	if (stack[fd])
		if (gnl_verify_line(&stack[fd], line))
			return (1);
	i = 0;
	while (i < BUFF_SIZE)
		heap[i++] = '\0';
	ret = gnl_read_file(fd, heap, &stack[fd], line);
	free(heap);
	if (ret != 0 || stack[fd] == NULL || stack[fd][0] == '\0')
	{
		if (!ret && *line)
			*line = NULL;
		return (ret);
	}
	*line = stack[fd];
	stack[fd] = NULL;
	return (1);
}
