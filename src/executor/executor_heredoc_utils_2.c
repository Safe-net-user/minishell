/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_heredoc_utils_2.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/17 12:19:22 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/17 12:24:40 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <unistd.h>
#include "ft_strings.h"
#include "heredoc.h"
#include "ft_io.h"
#include "expander.h"

static char	*expand_hd_line(t_mms *mms, char *line)
{
	t_tk	tmp;
	t_tk	*tk_ptr;
	char	*result;

	tmp.value = line;
	tmp.type_tk = TOK_WORD;
	tmp.flags = 0;
	tmp.next = NULL;
	tmp.prev = NULL;
	tk_ptr = &tmp;
	if (expand_one(mms, &tk_ptr) != EXP_SUCCESS)
		return (NULL);
	if (!tk_ptr)
		return (ft_strdup(""));
	result = tk_ptr->value;
	if (!result)
		return (ft_strdup(""));
	return (result);
}

static int	write_hd_line(int fd_out, char *line, bool expand, t_mms *mms)
{
	char	*out;

	if (expand)
	{
		out = expand_hd_line(mms, line);
		if (!out || write(fd_out, out, ft_strlen(out)) == -1)
		{
			free(out);
			return (-1);
		}
		free(out);
		return (0);
	}
	if (write(fd_out, line, ft_strlen(line)) == -1)
		return (-1);
	return (0);
}

static char	*read_hd_raw(t_mms *mms, int *stop)
{
	char	*line;
	int		interrupted;

	write(STDOUT_FILENO, "> ", 2);
	line = heredoc_gnl(mms->tty_fd, &interrupted);
	if (interrupted)
	{
		free(line);
		*stop = -2;
		return (NULL);
	}
	if (!line)
	{
		ft_putstr_fd("minishell: warning: here-document delimited "
			"by end-of-file\n", STDERR_FILENO);
		*stop = 1;
		return (NULL);
	}
	*stop = 0;
	return (line);
}

static char	*read_hd_line(t_mms *mms, char *lim_nl, int *stop)
{
	char	*line;

	line = read_hd_raw(mms, stop);
	if (*stop != 0)
		return (NULL);
	if (line_matches_delim(line, lim_nl))
	{
		free(line);
		*stop = 1;
		return (NULL);
	}
	return (line);
}

int	execute_here_doc(t_mms *mms, int *fd_here_doc, char *lim_nl, bool expand)
{
	char	*line;
	int		stop;
	int		ret;

	line = read_hd_line(mms, lim_nl, &stop);
	if (stop != 0)
		return (stop);
	ret = write_hd_line(fd_here_doc[1], line, expand, mms);
	free(line);
	if (ret == -1)
		return (-1);
	return (0);
}
