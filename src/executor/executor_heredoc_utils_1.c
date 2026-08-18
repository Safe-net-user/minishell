/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_heredoc_utils1.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 00:58:17 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/25 01:23:56 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heredoc.h"
#include "ft_strings.h"

size_t	hd_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s && s[i])
		i++;
	return (i);
}

char	*hd_strchr(const char *s, int c)
{
	while (s && *s)
	{
		if (*s == (char)c)
			return ((char *)s);
		s++;
	}
	if (s && (char)c == '\0')
		return ((char *)s);
	return (NULL);
}

char	*hd_strjoin_free(char *s1, char *s2)
{
	char	*res;
	size_t	len1;
	size_t	len2;
	size_t	i;

	len1 = hd_strlen(s1);
	len2 = hd_strlen(s2);
	res = malloc(len1 + len2 + 1);
	if (!res)
		return (NULL);
	i = 0;
	while (i < len1)
	{
		res[i] = s1[i];
		i++;
	}
	i = 0;
	while (i < len2)
	{
		res[len1 + i] = s2[i];
		i++;
	}
	res[len1 + len2] = '\0';
	free(s1);
	return (res);
}

int	line_matches_delim(char *line, char *lim)
{
	size_t	len;

	len = ft_strlen(line);
	if (len > 0 && line[len - 1] == '\n')
		len--;
	return (ft_strlen(lim) == len && ft_strncmp(line, lim, len) == 0);
}
