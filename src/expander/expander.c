/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 01:27:50 by gaspard           #+#    #+#             */
/*   Updated: 2026/08/20 23:58:02 by gd-hallu         ###   ########.fr       */
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

static bool	is_redir_operator(t_type_tk type)
{
	return (type == TOK_LESS || type == TOK_GREAT
		|| type == TOK_DLESS || type == TOK_DGREAT
		|| type == TOK_LESSAND || type == TOK_GREATAND
		|| type == TOK_LESSGREAT || type == TOK_DLESSDASH
		|| type == TOK_CLOBBER);
}

/*
** Parcourt et expand tous les tokens TOK_WORD / TOK_DELIMITER de la
** liste. head est l'adresse du pointeur de tete (ex: &node->tokens)
** pour repercuter une eventuelle suppression du premier token.
*/
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

static t_val_exp	expand_redirection(t_mms *mms, t_tk **op_ref)
{
	t_tk	**file_ref;

	if (!*op_ref || !is_redir_operator((*op_ref)->type_tk)
		|| !(*op_ref)->next)
		return (EXP_SUCCESS);
	if ((*op_ref)->type_tk == TOK_DLESS
		|| (*op_ref)->type_tk == TOK_DLESSDASH)
	{
		*op_ref = (*op_ref)->next->next;
		return (EXP_SUCCESS);
	}
	file_ref = &(*op_ref)->next;
	if (expand_one(mms, file_ref) != EXP_SUCCESS)
		return (EXP_ERROR);
	if (*file_ref)
		*op_ref = (*file_ref)->next;
	else
		*op_ref = NULL;
	return (EXP_SUCCESS);
}

/*
** Parcourt la liste operateur/fichier de node->redirect.
** Saute l'expansion du delimiteur pour << et <<- (jamais expanse).
** head est l'adresse du pointeur de tete (ex: &node->redirect).
*/
t_val_exp	expand_redirections(t_mms *mms, t_tk **head)
{
	t_tk	*op;

	if (!mms || !head || !*head)
		return (EXP_SUCCESS);
	op = *head;
	while (op)
	{
		if (expand_redirection(mms, &op) != EXP_SUCCESS)
			return (EXP_ERROR);
	}
	return (EXP_SUCCESS);
}
