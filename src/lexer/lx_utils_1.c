/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lx_utils_1.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 03:48:05 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/25 03:50:04 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

bool	is_no_value_tk(t_type_tk type)
{
	return (type == TOK_PIPE || type == TOK_AND_IF
		|| type == TOK_OR_IF || type == TOK_AMPERSAND
		|| type == TOK_LESS || type == TOK_GREAT
		|| type == TOK_DGREAT || type == TOK_DLESS);
}
