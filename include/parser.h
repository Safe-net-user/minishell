/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 10:27:34 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/06/25 12:09:37 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

#include "lexer.h"
#include "ft_stack_alloc.h"
#include "lexer.h"

typedef enum e_node_type
{
    NODE_CMD,
    NODE_PIPE,
    NODE_AND,
    NODE_OR,
    NODE_REDIR
} t_node_type;


typedef struct s_ast
{
    t_node_type     type;       // type du noeud AST
    t_type_token    tok_type;   // type du token lexer (TOK_LESS, TOK_GREAT, etc.)
    t_flag_token    *flags;      // flags du token lexer (SQUOTED, DQUOTED, etc.)
    struct s_ast    *left;
    struct s_ast    *right;
    union
    {
        char    **argv;   // NODE_CMD (par ex [echo] [-n] [hello]) donc avancer while (tok_type == WORD)
        char    *value;   // NODE_REDIR  (délimiteur) ou par ex juste "&&"
        //t_ast *subshell;
    };
}   t_ast;


#endif