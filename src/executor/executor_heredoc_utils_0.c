/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_heredoc_utils_0.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 00:56:15 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/25 01:48:41 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heredoc.h"
#include <errno.h>

static char	*g_hd_buf = NULL;

static int	hd_fill_buffer(int fd)
{
	char	read_buf[HD_BUFFER_SIZE + 1];
	ssize_t	bytes_read;

	bytes_read = 1;
	while (!hd_strchr(g_hd_buf, '\n') && bytes_read > 0)
	{
		bytes_read = read(fd, read_buf, HD_BUFFER_SIZE);
		if (bytes_read == -1 && errno == EINTR)
			return (-2);
		if (bytes_read == -1)
		{
			free(g_hd_buf);
			g_hd_buf = NULL;
			return (-1);
		}
		if (bytes_read == 0)
			break ;
		read_buf[bytes_read] = '\0';
		g_hd_buf = hd_strjoin_free(g_hd_buf, read_buf);
		if (!g_hd_buf)
			return (-1);
	}
	return (0);
}

static char	*hd_extract_line(char *buf)
{
	char	*nl;
	char	*line;
	size_t	len;
	size_t	i;

	if (!buf || !buf[0])
		return (NULL);
	nl = hd_strchr(buf, '\n');
	if (nl)
		len = (size_t)(nl - buf) + 1;
	else
		len = hd_strlen(buf);
	line = malloc(len + 1);
	if (!line)
		return (NULL);
	i = 0;
	while (i < len)
	{
		line[i] = buf[i];
		i++;
	}
	line[i] = '\0';
	return (line);
}

static char	*hd_extract_rest(char *buf)
{
	char	*nl;
	char	*rest;
	size_t	i;

	nl = hd_strchr(buf, '\n');
	if (!nl)
		return (NULL);
	rest = malloc(hd_strlen(nl + 1) + 1);
	if (!rest)
		return (NULL);
	i = 0;
	while (nl[i + 1])
	{
		rest[i] = nl[i + 1];
		i++;
	}
	rest[i] = '\0';
	return (rest);
}

char	*heredoc_gnl(int fd, int *interrupted)
{
	char	*line;
	char	*tmp;
	int		res;

	res = hd_fill_buffer(fd);
	if (interrupted)
		*interrupted = (res == -2);
	if (res == -1 || res == -2)
		return (NULL);
	if (!g_hd_buf || !g_hd_buf[0])
	{
		free(g_hd_buf);
		g_hd_buf = NULL;
		return (NULL);
	}
	line = hd_extract_line(g_hd_buf);
	tmp = g_hd_buf;
	g_hd_buf = hd_extract_rest(g_hd_buf);
	free(tmp);
	return (line);
}

void	heredoc_gnl_reset(void)
{
	free(g_hd_buf);
	g_hd_buf = NULL;
}