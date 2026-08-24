/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/24 15:59:19 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/08/22 18:26:11 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTIN_H
# define BUILTIN_H

# include "env.h"
# include <stddef.h>

typedef enum e_builtins_val
{
	BUI_SUCCESS,
	BUI_ERROR,
	BUI_TOO_MANY_ARGS,
}	t_builts_val;

static inline void	swap_addr(t_env_entry *a, t_env_entry *b)
{
	t_env_entry	temp;

	temp = *b;
	*b = *a;
	*a = temp;
}

/* ------------- builtin functions -------------*/
t_builts_val	builtin_cd(t_mms *mms, char **argv);
t_builts_val	builtin_echo(char **tab);
t_builts_val	builtin_env(t_env *env, char **args);
t_builts_val	builtin_exit(t_mms *mms, char **args);
t_builts_val	builtin_export(t_mms *mms, char **arg);
t_builts_val	builtin_pwd(t_mms *mms);
t_builts_val	builtin_unset(t_mms *mms, char **argv);

/* ------------- utils functions -------------*/
int				is_validname(char *str);
void			ht_to_arr(t_env *env, t_env_entry *arr);
size_t			len_arr(t_env_entry *entries);
int				sort_arr(t_env_entry *entries);
int				print_exported_var(t_env *env);
int				alone_key_job(t_mms *mms, char *buffer_key);
t_builts_val	copy_key(char *dst, char *src, size_t *i);
t_builts_val	copy_value(char *dst, char *src, size_t *i);
t_env_entry		*build_sorted_arr(t_env *env);
int				export_internal_error(t_env_entry *arr);
char			*get_new_path(t_mms *mms, char *old_pwd, char *path);
t_builts_val	too_many_args_error(void);
t_builts_val	variable_not_set_error(void);
t_builts_val	internal_error(void);

#endif
