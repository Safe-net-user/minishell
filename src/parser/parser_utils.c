/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 14:55:25 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/12 13:28:05 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

bool	is_command_token(t_type_tk type)
{
	return (type == TOK_WORD
		|| type == TOK_LESS
		|| type == TOK_DLESS
		|| type == TOK_GREAT
		|| type == TOK_DGREAT
		|| type == TOK_DELIMITER);
}

void	init_cmd_node(t_ast *node)
{
	node->type = NODE_CMD;
	node->left = NULL;
	node->right = NULL;
	node->redirect = NULL;
	node->tokens = NULL;
}
