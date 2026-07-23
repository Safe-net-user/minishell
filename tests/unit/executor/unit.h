#ifndef UNIT_H
# define UNIT_H

# define TEST_DIR "/tmp/minishell_executor_tests"

# include "../../../include/executor.h"
# include "../../../include/minishell.h"
# include "../../../include/parser.h"
# include "../../../lib/libft/include/ft_stack_alloc.h"
# include "../../../lib/libft/include/ft_hashtable.h"

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <errno.h>

typedef struct s_capture
{
	char	*stdout_data;
	char	*stderr_data;
	size_t	stdout_len;
	size_t	stderr_len;
	int		status;
}	t_capture;

/*
 * INITIALISATION
 */

void		init_test_mms(t_mms *mms);
void		free_test_mms(t_mms *mms);

/*
 * AST BUILDERS
 */

t_tk		*make_token(t_stack_alloc *sa, char *value, t_type_tk type);
t_ast		*make_cmd(t_stack_alloc *sa, char **argv);
t_ast		*make_pipe(t_stack_alloc *sa, t_ast *left, t_ast *right);

t_redir		*make_redir(t_stack_alloc *sa,
				t_type_tk type, char *file);
void		add_redir(t_ast *ast, t_redir *redir);

/*
 * TEST UTILITIES
 */

int			capture_executor(t_mms *mms, t_ast *ast,
				t_capture *capture);
int			capture_bash(char *command, t_capture *capture);

int			compare_captures(t_capture *expected,
				t_capture *actual);

#endif