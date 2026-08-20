/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_heredoc_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 14:19:45 by miouali           #+#    #+#             */
/*   Updated: 2026/08/20 14:22:32 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"
#include <unistd.h>
#include "ft_strings.h"

int	write_heredoc_content(int fd[2], char *content)
{
	ssize_t	len;

	len = write(fd[1], content, ft_strlen(content));
	if (len == -1)
	{
		perror("miniMishell");
		close(fd[0]);
		close(fd[1]);
		return (1);
	}
	close(fd[1]);
	return (0);
}
