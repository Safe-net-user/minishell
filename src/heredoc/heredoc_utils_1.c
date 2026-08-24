/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils_1.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 16:05:11 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/24 16:05:32 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heredoc.h"
#include "expander.h"

void	strip_quotes(char *s)
{
	size_t	len;

	len = ft_strlen(s);
	if (len < 2)
		return ;
	if ((s[0] == '\'' && s[len - 1] == '\'')
		|| (s[0] == '"' && s[len - 1] == '"'))
	{
		ft_memmove(s, s + 1, len - 2);
		s[len - 2] = '\0';
	}
}

char	*expand_hd_line(t_mms *mms, char *line)
{
	t_tk	tmp;
	t_tk	*tk_ptr;

	tmp.value = line;
	tmp.type_tk = TOK_WORD;
	tmp.flags = 0;
	tmp.next = NULL;
	tmp.prev = NULL;
	tmp.heredoc_content = NULL;
	tk_ptr = &tmp;
	if (expand_one(mms, &tk_ptr) != EXP_SUCCESS)
		return (NULL);
	if (!tk_ptr || !tk_ptr->value)
		return (ft_strdup(""));
	return (tk_ptr->value);
}