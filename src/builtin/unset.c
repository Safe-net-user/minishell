#include "env.h"
#include "minishell.h"
#include "builtin.h"
#include "ft_strings.h"

t_builts_val	builtin_unset(t_mms *mms, char **argv)
{
	t_env_entry		*entry;
	t_builts_val	ret;
	size_t			i;

	if (!mms || !mms->env || !argv)
		return (BUI_ERROR);
	ret = BUI_SUCCESS;
	i = 1;
	while (argv[i])
	{
		entry = get_env(mms->env, argv[i]);
		if (entry)
		{
			if (entry->flags & READONLY)
				ret = BUI_READONLY_VAR;
			else
				del_env(mms->env, argv[i]);
			if (ft_strcmp(entry->key, "PATH") == 0)
			{
				free_hash_table(mms->cmd_path);
				mms->cmd_path = init_hash_table(INIT_SIZE_HT);
				if (!mms->cmd_path)
					return (BUI_ERROR);
			}
		}
		i++;
	}
	return (ret);
}
