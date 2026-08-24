/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/24 16:00:53 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/20 23:54:48 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXPANDER_H
# define EXPANDER_H

# include "minishell.h"
# include "lexer.h"
# include "ft_string_builder.h"
# include <stddef.h>

typedef enum e_state_exp
{
	ST_EXP_NORMAL,
	ST_EXP_SQUOTE,
	ST_EXP_DQUOTE,
}	t_state_exp;

typedef enum e_val_exp
{
	EXP_SUCCESS,
	EXP_ERROR,
	EXP_LEN_VAR,
}	t_val_exp;

typedef struct s_exp
{
	char		*str;
	t_sb		*sb;
	t_mms		*mms;
	size_t		index;
	t_state_exp	state;
	char		padding[4];
}	t_exp;

typedef t_val_exp	(*t_exp_variant_fn)(t_exp *);

t_val_exp	expand_one(t_mms *mms, t_tk **tk_ref);
t_val_exp	expand_tokens(t_mms *mms, t_tk **head);
t_val_exp	expand_redirections(t_mms *mms, t_tk **head);
t_val_exp	expand_one(t_mms *mms, t_tk **tk_ref);
t_val_exp	exp_dquote(t_exp *exp);
t_val_exp	exp_append_expansion(t_exp *exp);
t_val_exp	exp_squote(t_exp *exp);
t_val_exp	exp_normal(t_exp *exp);
t_val_exp	exp_append_expansion(t_exp *exp);
void		init_lut(t_exp_variant_fn *lut);
t_exp		*init_expander(t_mms *mms);
void		reset_expander(t_exp *exp, char *str);
void		init_lut(t_exp_variant_fn *lut);
t_exp		*init_expander(t_mms *mms);
t_val_exp	expand_word_failed(t_exp *exp);
void		empty_token(t_tk *tk, t_tk **tk_ref);
t_val_exp	new_value_failed(t_exp *exp);
void		unlink_token(t_tk **tk_ref);
t_val_exp	end_expand_one(t_exp *exp);
void		free_fill(t_tk *tk, char *str);
void		unlink_token(t_tk **tk_ref);

#endif
