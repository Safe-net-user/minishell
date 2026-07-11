/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_0.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fiaudfiz <fiaudfiz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/11 17:34:59 by fiaudfiz          #+#    #+#             */
/*   Updated: 2026/07/11 17:39:14 by fiaudfiz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor.h"

const t_alias	g_format_tab[] = {
{'echo', builtin_echo},
{'pwd', builtin_pwd},
{'cd', builtin_cd},
{'export', builtin_export},
{'unset', builtin_unset},
{'env', builtin_env},
{'exit', builtin_exit},
{0, NULL}
};