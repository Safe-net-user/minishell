/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 10:27:34 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/20 14:48:24 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "lexer.h"
# include "ft_stack_alloc.h"
# include <stdbool.h>
# include <stdio.h>

typedef enum e_node_type
{
	NODE_CMD,
	NODE_PIPE,
	NODE_AND,
	NODE_OR,
	NODE_REDIR
}	t_node_type;

//au debut du parser, faire les next et les prev, modifier la struct du parser et la strcut de redirection puis ajoputer << 1 par ex et enfin refaire l'executor avec le .md

typedef struct s_ast
{
	t_node_type		type;
	struct s_ast	*left;
	struct s_ast	*right;
	t_tk			*redirect;
	t_tk			*tokens;
}	t_ast;

t_tk		*next_token(t_tk *cur);
int			count_tokens(t_tk *token);
bool		is_command_token(t_type_tk type);
void		init_cmd_node(t_ast *node);

char		*token_to_string(t_type_tk type);
void		parser_error(t_mms *mms, t_tk *tok);

bool		parse_redirection(t_mms *mms, t_tk **token, t_ast *node);
t_ast		*parse_command(t_mms *mms, t_tk **token);
t_ast		*parse_pipe(t_mms *mms, t_tk **token);
t_ast		*parse_or_and(t_mms *mms, t_tk **token);
void		add_redirect(t_ast *node, t_tk *new_op);
bool		parse_heredoc(t_mms *mms, t_tk *op, t_tk *file);
t_tk		*new_redir_token(t_mms *mms, t_tk *src);

t_ast		*parser(t_mms *mms);

#endif
