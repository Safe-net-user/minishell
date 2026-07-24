#include "builtin.h"
#include "ft_strings.h"
#include "minishell.h"
#include "ft_stdlib.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int is_valid_arg(char *str)
{
	size_t i;

	i = 0;
	if (!str)
		return (0);
	if ((str[0] == '+'|| str[0] == '-') && ft_isdigit(str[1]))
		i++;
	while (str[i])
	{
		if (!ft_isdigit(str[i++]))
			return (0);
	}
	return (1);
}

t_builts_val builtin_exit(t_mms *mms, char **args)
{
	int last_status;

	if (!mms || !args)
		return (BUI_ERROR);
	last_status = mms->last_status;
	if (args[1] && args[2])
		return (BUI_TOO_MANY_ARGS);
	if (!args[1])
	{
		printf("exit\n");
		free_og_struct(mms);
		exit(last_status);
	}
	else if (is_valid_arg(args[1]))
	{
		printf("exit\n");
		free_og_struct(mms);
		exit(ft_atoi(args[1]) % 256);
	}
	else
	{
		printf("exit\n");
		free_og_struct(mms);
		exit(255);
	}
	return (BUI_SUCCESS);
}
