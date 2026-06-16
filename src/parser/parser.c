/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/16 10:27:18 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/06/16 10:30:28 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
    t_node_type type;
    struct s_ast *left;
    struct s_ast *right;
   union u_node
    {
    t_cmd cmd;
    t_redir redir;
    //t_ast *subshell;
    };
} t_ast;