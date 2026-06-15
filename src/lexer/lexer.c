/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 11:33:11 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/06/15 11:08:27 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

bool rule_op_continue(t_mms *mms, int *state, char *cmdl, t_sb *sb)
{
    t_token *token;
    
    if (!sb || !mms || !state || !cmdl)
        return (0);
    if ((*state & ST_OPERATOR_AND && cmdl[0] == '&') || *state & ST_OPERATOR_OR && cmdl[0] == '|')
    {
        append_sb(sb, cmdl[0]);
        if (cmdl[0] == '&')
            token->type_tk =  TOK_AND_IF;
        else
            token->type_tk =  TOK_OR_IF;
        token->value = assign_sb(sb);
        token->flags = 0;
        return (1);
    }
    return (0);
}

int    lexer(char *cmdl, t_mms *mms)
{
    int             state;
    size_t          i;
    t_sb            *sb;

    i = 0;
    state = ST_NORMAL;
    sb = init_sb(INIT_SIZE_SB);
    if (!mms || !cmdl || !sb)
        return (0);
    while (cmdl[i])
    {
        if (rule_op_continue(mms, &state, &cmdl[i], sb) == 1)
            continue ;
        i++;
    }
    return (1);
}