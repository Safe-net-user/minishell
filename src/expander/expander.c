/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miouali <miouali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 01:27:50 by gaspard           #+#    #+#             */
/*   Updated: 2026/08/20 16:03:03 by miouali          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file expander.c
 * @brief Word expansion driver — resolves a token's raw value into
 *        its final, expanded string.
 *
 * `expand_one()` runs a small state-machine automaton (`t_exp`, 3
 * states: normal/squote/dquote) over a single token's value via
 * `expand_word()`, dispatching each character through a lookup
 * table of per-state handlers. If expansion yields an empty,
 * unquoted result on a token that came from an expansion, the token
 * is unlinked from its list (`unlink_token()`) — mimicking the shell
 * behavior where `"$EMPTY"` disappears entirely rather than leaving
 * a blank argument.
 *
 * `expand_tokens()` applies this to every TOK_WORD/TOK_DELIMITER in
 * a command's argument list, while `expand_redirections()` applies
 * it only to redirection targets — explicitly skipping the
 * delimiter of `<<`/`<<-`, which must never be expanded at parse
 * time.
 */

#include "minishell.h"
#include "expander.h"
#include "ft_hashtable.h"
#include "ft_strings.h"
#include "ft_stdlib.h"
#include <stdlib.h>

static t_val_exp	expand_word(t_exp *exp, t_exp_variant_fn *lut)
{
	t_val_exp	ret;

	if (!exp || !lut)
		return (EXP_ERROR);
	while (exp->str[exp->index])
	{
		ret = lut[exp->state](exp);
		if (ret != EXP_SUCCESS)
			return (ret);
	}
	return (EXP_SUCCESS);
}

void	unlink_token(t_tk **tk_ref)
{
	t_tk	*tk;
	t_tk	*next;
	t_tk	*prev;

	tk = *tk_ref;
	next = tk->next;
	prev = tk->prev;
	if (prev)
		prev->next = next;
	if (next)
		next->prev = prev;
	*tk_ref = next;
}

static bool	is_redir_operator(t_type_tk type)
{
	return (type == TOK_LESS || type == TOK_GREAT
		|| type == TOK_DLESS || type == TOK_DGREAT
		|| type == TOK_LESSAND || type == TOK_GREATAND
		|| type == TOK_LESSGREAT || type == TOK_DLESSDASH
		|| type == TOK_CLOBBER);
}

t_val_exp	expand_one(t_mms *mms, t_tk **tk_ref)
{
	t_exp				*exp;
	t_exp_variant_fn	lut[3];
	t_tk				*tk;
	char				*new_value;

	tk = *tk_ref;
	if (!tk->value)
		return (EXP_SUCCESS);
	exp = init_expander(mms);
	if (!exp)
		return (EXP_ERROR);
	init_lut(lut);
	reset_expander(exp, tk->value);
	if (expand_word(exp, lut) != EXP_SUCCESS)
	{
		free_sb(exp->sb);
		free(exp);
		return (EXP_ERROR);
	}
	if (!exp->sb->str[0])
	{
		free(tk->value);
		tk->value = '\0';
		if (!(tk->flags & TOKF_SQUOTE) && (!(tk->flags & TOKF_DQUOTE) && tk->flags & TOKF_EXPANSION))
			unlink_token(tk_ref);
	}
	else
	{
		new_value = ft_strdup(exp->sb->str);
		if (!new_value)
		{
			free_sb(exp->sb);
			free(exp);
			return (EXP_ERROR);
		}
		free(tk->value);
		tk->value = new_value;
	}
	free_sb(exp->sb);
	free(exp);
	return (EXP_SUCCESS);
}

t_val_exp	expand_tokens(t_mms *mms, t_tk **head)
{
	t_tk	*cur;
	t_tk	**ref;

	if (!mms || !head || !*head)
		return (EXP_SUCCESS);
	ref = head;
	while (*ref)
	{
		cur = *ref;
		if ((*ref)->type_tk == TOK_WORD || (*ref)->type_tk == TOK_DELIMITER)
		{
			if (expand_one(mms, ref) != EXP_SUCCESS)
				return (EXP_ERROR);
		}
		if (*ref == cur)
			ref = &(*ref)->next;
	}
	return (EXP_SUCCESS);
}

t_val_exp	expand_redirections(t_mms *mms, t_tk **head)
{
	t_tk	*op;
	t_tk	**file_ref;

	if (!mms || !head || !*head)
		return (EXP_SUCCESS);
	op = *head;
	while (op)
	{
		if (!is_redir_operator(op->type_tk) || !op->next)
		{
			op = op->next;
			continue ;
		}
		if (op->type_tk == TOK_DLESS || op->type_tk == TOK_DLESSDASH)
		{
			op = op->next->next;
			continue ;
		}
		file_ref = &op->next;
		if (expand_one(mms, file_ref) != EXP_SUCCESS)
			return (EXP_ERROR);
		op = *file_ref ? (*file_ref)->next : NULL;
	}
	return (EXP_SUCCESS);
}