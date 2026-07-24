#include "builtin.h"
#include <stdio.h>

static t_builts_val print_exported_var(t_env *env)
{
	t_env_entry *entry;
	t_env_entry *limit;

	entry = env->indexes;
	limit = env->indexes + env->capacity;
	while (entry < limit)
	{
		if (entry->key != NULL
			&& entry->key != DELETED
			&& (entry->flags & EXPORTED)
			&& entry->value != NULL)
		{
			printf("%s=%s\n", entry->key, entry->value);
		}
		entry++;
	}
	return (BUI_SUCCESS);
}

t_builts_val builtin_env(t_env *env, char **args)
{
	if (!env || !args)
		return (BUI_ERROR);
	if (args[1])
		return (BUI_TOO_MANY_ARGS);
	return (print_exported_var(env));
}
