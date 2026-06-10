/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gd-hallu <gd-hallu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 14:01:35 by gd-hallu          #+#    #+#             */
/*   Updated: 2026/06/10 20:23:38 by gd-hallu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"
#include "ft_string_builder.h"

static int sb_alloc_part(t_sb **sb1, t_sb **sb2)
{
    *sb1 = init_sb(64);
	if (!*sb1)
		return (0);
	*sb2 = init_sb(128);
	if (!*sb2)
	{
		free_sb(*sb1);
		return (0);
	}
    return (1);
}

static int  sb_free_part(t_sb *sb1, t_sb *sb2)
{
    if (sb1)
        free_sb(sb1);
    if (sb2)
        free_sb(sb2);
    return (0);
}

int env_arr_to_ht(char **str, t_env *ht)
{
	int		i;
	int		j;
	int		flags;
	t_sb	*sb1;
	t_sb	*sb2;
	
	i = 0;
	flags = 0;
	sb1 = NULL;
	sb2 = NULL;
	while(str[i])
	{
        j = 0;
        if (sb_alloc_part(&sb1, &sb2) == 0)
            return (0);
		while (str[i][j] && str[i][j] != '=')
			if (append_sb(sb1, &str[i][j++]) == 0)
                return (sb_free_part(sb1, sb2));
		j++;
		while (str[i][j])
			if (append_sb(sb2, &str[i][j++]) == 0)
                return (sb_free_part(sb1, sb2));
		add_env(ht, sb1->str, sb2->str, flags |= EXPORTED);
		sb_free_part(sb1, sb2);
		i++;
	}
	return (1);
}