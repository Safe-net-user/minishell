# include "builtin.h"
#include <stddef.h>
#include "env.h"
#include "ft_strings.h"

int is_validname(char *str)
{
	if (!str)
		return (0);
	if (ft_isdigit(str[0]) || ft_isalpha(str[0]) || str[0] == '_')
		return (1);
	return (0);
}
void ht_to_arr(t_env *env, t_env_entry *arr)
{
	t_env_entry *entry;
	t_env_entry *limit;
	t_env_entry *curr_p;

	entry = env->indexes;
	limit = env->indexes + env->capacity;
	curr_p = arr;
	while (entry < limit)
	{
		if (entry->key != NULL && entry->key != DELETED && entry->flags & EXPORTED)
		{
			*curr_p = *entry;
			curr_p++;
		}
		entry++;
	}
	curr_p->key = NULL;
}

size_t len_arr(t_env_entry *entries)
{
	size_t count;

	count = 0;
	while (entries->key)
	{
		count++;
		entries++;
	}
	return (count);
}

int sort_arr(t_env_entry *entries)
{
	size_t i;
	size_t j;
	t_env_entry *limit;
	size_t n;

	if (!entries)
		return (0);
	n = len_arr(entries);
	i = 0;
	limit = entries + n;
	while (entries + i < limit)
	{
		j = i + 1;
		while (entries + j < limit - 1)
		{
			if (ft_strcmp(entries[i].key, entries[j].key) > 0)
				swap_addr(&entries[i], &entries[j]);
			j++;
		}
		i++;
	}
	return (1);
}
