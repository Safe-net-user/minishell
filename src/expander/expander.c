/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 01:27:50 by gaspard           #+#    #+#             */
/*   Updated: 2026/08/20 13:15:30 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

/*
** Detache *tk_ref de sa liste chainee et met a jour tk_ref pour
** pointer sur le token suivant (ou NULL). Met a jour prev/next
** des voisins pour garder la liste coherente.
*/
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
		if (!(tk->flags & TOKF_SQUOTE) && (!(tk->flags & TOKF_DQUOTE) \
&& tk->flags & TOKF_EXPANSION))
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

/*
** Parcourt la liste operateur/fichier de node->redirect.
** Saute l'expansion du delimiteur pour << et <<- (jamais expanse).
** head est l'adresse du pointeur de tete (ex: &node->redirect).
*/
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
		if (*file_ref)
			op = (*file_ref)->next;
		else
			op = NULL;
	}
	return (EXP_SUCCESS);
}