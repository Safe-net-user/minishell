#include "unit.h"

static int	g_test_number;
static int	g_failed;

/*
 * ============================================================================
 * UTILS
 * ============================================================================
 */

static void	print_result(char *name, int result)
{
	g_test_number++;
	if (result == 0)
		printf("[PASS] %03d - %s\n", g_test_number, name);
	else
	{
		printf("[FAIL] %03d - %s\n", g_test_number, name);
		g_failed++;
	}
}

static void	free_capture(t_capture *capture)
{
	if (capture->stdout_data)
		free(capture->stdout_data);
	if (capture->stderr_data)
		free(capture->stderr_data);
	memset(capture, 0, sizeof(*capture));
}

static char	*read_fd(int fd, size_t *len)
{
	char	*buffer;
	char	tmp[4096];
	ssize_t	n;
	size_t	capacity;
	size_t	total;

	capacity = 4096;
	total = 0;
	buffer = malloc(capacity);
	if (!buffer)
		return (NULL);
	while (1)
	{
		n = read(fd, tmp, sizeof(tmp));
		if (n <= 0)
			break ;
		if (total + n + 1 > capacity)
		{
			while (total + n + 1 > capacity)
				capacity *= 2;
			buffer = realloc(buffer, capacity);
			if (!buffer)
				return (NULL);
		}
		memcpy(buffer + total, tmp, n);
		total += n;
	}
	buffer[total] = '\0';
	*len = total;
	return (buffer);
}

static void	close_pipe(int pipefd[2])
{
	close(pipefd[0]);
	close(pipefd[1]);
}

/*
 * ============================================================================
 * CAPTURE EXECUTOR
 * ============================================================================
 */

int	capture_executor(t_mms *mms, t_ast *ast, t_capture *capture)
{
	int		out_pipe[2];
	int		err_pipe[2];
	pid_t	pid;
	int		status;

	memset(capture, 0, sizeof(*capture));

	if (pipe(out_pipe) == -1)
		return (1);
	if (pipe(err_pipe) == -1)
	{
		close_pipe(out_pipe);
		return (1);
	}

	pid = fork();
	if (pid == -1)
	{
		close_pipe(out_pipe);
		close_pipe(err_pipe);
		return (1);
	}

	if (pid == 0)
	{
		close(out_pipe[0]);
		close(err_pipe[0]);

		if (dup2(out_pipe[1], STDOUT_FILENO) == -1)
			_exit(255);
		if (dup2(err_pipe[1], STDERR_FILENO) == -1)
			_exit(255);

		close(out_pipe[1]);
		close(err_pipe[1]);

		_exit(executor(mms, ast));
	}

	close(out_pipe[1]);
	close(err_pipe[1]);

	capture->stdout_data = read_fd(out_pipe[0],
			&capture->stdout_len);
	capture->stderr_data = read_fd(err_pipe[0],
			&capture->stderr_len);

	close(out_pipe[0]);
	close(err_pipe[0]);

	waitpid(pid, &status, 0);

	if (WIFEXITED(status))
		capture->status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		capture->status = 128 + WTERMSIG(status);

	return (0);
}

/*
 * ============================================================================
 * CAPTURE BASH --POSIX
 * ============================================================================
 */

int	capture_bash(char *command, t_capture *capture)
{
	int		out_pipe[2];
	int		err_pipe[2];
	pid_t	pid;
	int		status;

	memset(capture, 0, sizeof(*capture));

	if (pipe(out_pipe) == -1)
		return (1);
	if (pipe(err_pipe) == -1)
	{
		close_pipe(out_pipe);
		return (1);
	}

	pid = fork();
	if (pid == -1)
	{
		close_pipe(out_pipe);
		close_pipe(err_pipe);
		return (1);
	}

	if (pid == 0)
	{
		close(out_pipe[0]);
		close(err_pipe[0]);

		dup2(out_pipe[1], STDOUT_FILENO);
		dup2(err_pipe[1], STDERR_FILENO);

		close(out_pipe[1]);
		close(err_pipe[1]);

		execl("/bin/bash", "bash", "--posix",
			"-c", command, NULL);
		_exit(255);
	}

	close(out_pipe[1]);
	close(err_pipe[1]);

	capture->stdout_data = read_fd(out_pipe[0],
			&capture->stdout_len);
	capture->stderr_data = read_fd(err_pipe[0],
			&capture->stderr_len);

	close(out_pipe[0]);
	close(err_pipe[0]);

	waitpid(pid, &status, 0);

	if (WIFEXITED(status))
		capture->status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		capture->status = 128 + WTERMSIG(status);

	return (0);
}

/*
 * ============================================================================
 * COMPARISON
 * ============================================================================
 */

static int	compare_output(char *name,
	char *expected, size_t expected_len,
	char *actual, size_t actual_len)
{
	if (expected_len != actual_len)
	{
		printf("\n%s length mismatch:\n", name);
		printf("  expected: %zu bytes\n", expected_len);
		printf("  actual:   %zu bytes\n", actual_len);
		return (1);
	}

	if (memcmp(expected, actual, expected_len) != 0)
	{
		printf("\n%s content mismatch:\n", name);
		printf("EXPECTED:\n[%s]\n", expected);
		printf("ACTUAL:\n[%s]\n", actual);
		return (1);
	}

	return (0);
}

int	compare_captures(t_capture *expected,
	t_capture *actual)
{
	if (expected->status != actual->status)
	{
		printf("\nSTATUS MISMATCH:\n");
		printf("  expected: %d\n", expected->status);
		printf("  actual:   %d\n", actual->status);
		return (1);
	}

	if (compare_output("STDOUT",
			expected->stdout_data,
			expected->stdout_len,
			actual->stdout_data,
			actual->stdout_len))
		return (1);

	if (compare_output("STDERR",
			expected->stderr_data,
			expected->stderr_len,
			actual->stderr_data,
			actual->stderr_len))
		return (1);

	return (0);
}

/*
 * ============================================================================
 * TEST RUNNER
 * ============================================================================
 */

static int	run_test(t_mms *mms, char *name,
	t_ast *ast, char *bash_command)
{
	t_capture	expected;
	t_capture	actual;
	int			result;

	capture_bash(bash_command, &expected);
	capture_executor(mms, ast, &actual);

	result = compare_captures(&expected, &actual);

	print_result(name, result);

	free_capture(&expected);
	free_capture(&actual);

	return (result);
}

/*
 * ============================================================================
 * SIMPLE COMMANDS
 * ============================================================================
 */

static void	test_simple_commands(t_mms *mms)
{
	t_ast	*ast;

	{
		char *a[] = {"true", NULL};

		ast = make_cmd(mms->sa, a);
		run_test(mms, "true", ast, "true");
	}

	{
		char *a[] = {"echo", "hello", NULL};

		ast = make_cmd(mms->sa, a);
		run_test(mms, "echo hello", ast, "echo hello");
	}

	{
		char *a[] = {"echo", "hello", "world", NULL};

		ast = make_cmd(mms->sa, a);
		run_test(mms, "echo multiple arguments",
			ast, "echo hello world");
	}

	{
		char *a[] = {"printf", "hello", NULL};

		ast = make_cmd(mms->sa, a);
		run_test(mms, "printf without newline",
			ast, "printf hello");
	}

	{
		char *a[] = {"printf", "%s", "hello", NULL};

		ast = make_cmd(mms->sa, a);
		run_test(mms, "printf format",
			ast, "printf '%s' hello");
	}

	{
		char *a[] = {"printf", "%d", "42", NULL};

		ast = make_cmd(mms->sa, a);
		run_test(mms, "printf integer",
			ast, "printf '%d' 42");
	}

	{
		char *a[] = {"/bin/echo", "absolute", NULL};

		ast = make_cmd(mms->sa, a);
		run_test(mms, "absolute path",
			ast, "/bin/echo absolute");
	}

	{
		char *a[] = {"pwd", NULL};

		ast = make_cmd(mms->sa, a);
		run_test(mms, "pwd",
			ast, "pwd");
	}

	{
		char *a[] = {"echo", "", NULL};

		ast = make_cmd(mms->sa, a);
		run_test(mms, "empty argument",
			ast, "echo ''");
	}

	{
		char *a[] = {"echo", "hello world", NULL};

		ast = make_cmd(mms->sa, a);
		run_test(mms, "argument containing spaces",
			ast, "echo 'hello world'");
	}
}

/*
 * ============================================================================
 * PIPELINES
 * ============================================================================
 */

static t_ast	*make_pipeline(t_stack_alloc *sa,
	char ***commands, size_t count)
{
	t_ast	*ast;
	t_ast	*next;
	size_t	i;

	ast = make_cmd(sa, commands[0]);
	if (!ast)
		return (NULL);

	i = 1;
	while (i < count)
	{
		next = make_cmd(sa, commands[i]);
		if (!next)
			return (NULL);
		ast = make_pipe(sa, ast, next);
		if (!ast)
			return (NULL);
		i++;
	}
	return (ast);
}

static void	test_pipelines(t_mms *mms)
{
	t_ast	*ast;

	{
		char *a[] = {"echo", "hello", NULL};
		char *b[] = {"cat", NULL};
		char **p[] = {a, b};

		ast = make_pipeline(mms->sa, p, 2);
		run_test(mms, "echo | cat",
			ast, "echo hello | cat");
	}

	{
		char *a[] = {"echo", "hello", NULL};
		char *b[] = {"grep", "hello", NULL};
		char **p[] = {a, b};

		ast = make_pipeline(mms->sa, p, 2);
		run_test(mms, "echo | grep",
			ast, "echo hello | grep hello");
	}

	{
		char *a[] = {"echo", "hello", NULL};
		char *b[] = {"wc", "-l", NULL};
		char **p[] = {a, b};

		ast = make_pipeline(mms->sa, p, 2);
		run_test(mms, "echo | wc",
			ast, "echo hello | wc -l");
	}

	{
		char *a[] = {"printf", "hello\\nworld\\n", NULL};
		char *b[] = {"grep", "hello", NULL};
		char *c[] = {"wc", "-l", NULL};
		char **p[] = {a, b, c};

		ast = make_pipeline(mms->sa, p, 3);
		run_test(mms, "printf | grep | wc",
			ast,
			"printf 'hello\\nworld\\n' | grep hello | wc -l");
	}

	{
		char *a[] = {"echo", "hello", NULL};
		char *b[] = {"cat", NULL};
		char *c[] = {"cat", NULL};
		char *d[] = {"cat", NULL};
		char **p[] = {a, b, c, d};

		ast = make_pipeline(mms->sa, p, 4);
		run_test(mms, "four-command pipeline",
			ast,
			"echo hello | cat | cat | cat");
	}
}

/*
 * ============================================================================
 * OUTPUT REDIRECTIONS
 * ============================================================================
 */

static void	test_output_redirections(t_mms *mms)
{
	t_ast	*ast;
	char	*a[] = {"echo", "hello", NULL};

	unlink(TEST_DIR "/out.txt");

	ast = make_cmd(mms->sa, a);
	add_redir(ast, make_redir(mms->sa,
			TOK_GREAT, TEST_DIR "/out.txt"));

	run_test(mms,
		"echo > file",
		ast,
		"echo hello > " TEST_DIR "/out.txt");

	{
		int		fd;
		char	buffer[128];
		ssize_t	n;

		fd = open(TEST_DIR "/out.txt", O_RDONLY);
		n = read(fd, buffer, sizeof(buffer) - 1);
		close(fd);

		if (n >= 0)
			buffer[n] = '\0';

		print_result("redirected file content",
			n >= 0 && strcmp(buffer, "hello\n") == 0
			? 0 : 1);
	}
}

static void	test_append(t_mms *mms)
{
	t_ast	*ast;
	char	*a[] = {"printf", "second", NULL};

	int	fd = open(TEST_DIR "/append.txt",
			O_WRONLY | O_CREAT | O_TRUNC, 0644);
	write(fd, "first", 5);
	close(fd);

	ast = make_cmd(mms->sa, a);
	add_redir(ast, make_redir(mms->sa,
			TOK_DGREAT, TEST_DIR "/append.txt"));

	run_test(mms,
		"append redirection",
		ast,
		"printf second >> " TEST_DIR "/append.txt");
}

/*
 * ============================================================================
 * INPUT REDIRECTIONS
 * ============================================================================
 */

static void	test_input_redirections(t_mms *mms)
{
	t_ast	*ast;
	char	*a[] = {"cat", NULL};
	int		fd;

	fd = open(TEST_DIR "/input.txt",
			O_WRONLY | O_CREAT | O_TRUNC, 0644);
	write(fd, "hello\n", 6);
	close(fd);

	ast = make_cmd(mms->sa, a);
	add_redir(ast, make_redir(mms->sa,
			TOK_LESS, TEST_DIR "/input.txt"));

	run_test(mms,
		"cat < file",
		ast,
		"cat < " TEST_DIR "/input.txt");
}

/*
 * ============================================================================
 * MULTIPLE REDIRECTIONS
 * ============================================================================
 */

static void	test_multiple_redirections(t_mms *mms)
{
	t_ast	*ast;
	char	*a[] = {"echo", "hello", NULL};

	unlink(TEST_DIR "/first.txt");
	unlink(TEST_DIR "/second.txt");

	ast = make_cmd(mms->sa, a);

	add_redir(ast, make_redir(mms->sa,
			TOK_GREAT, TEST_DIR "/first.txt"));

	add_redir(ast, make_redir(mms->sa,
			TOK_GREAT, TEST_DIR "/second.txt"));

	run_test(mms,
		"multiple output redirections",
		ast,
		"echo hello > " TEST_DIR "/first.txt > "
		TEST_DIR "/second.txt");
}

/*
 * ============================================================================
 * MAIN
 * ============================================================================
 */

int	main(void)
{
	t_mms	mms;

	g_test_number = 0;
	g_failed = 0;

	init_test_mms(&mms);

	mkdir(TEST_DIR, 0755);

	printf("\n");
	printf("============================================================\n");
	printf("          EXECUTOR VS BASH --POSIX TEST SUITE\n");
	printf("============================================================\n\n");

	printf("---- SIMPLE COMMANDS ----\n");
	test_simple_commands(&mms);

	printf("\n---- PIPELINES ----\n");
	test_pipelines(&mms);

	printf("\n---- OUTPUT REDIRECTIONS ----\n");
	test_output_redirections(&mms);

	printf("\n---- APPEND ----\n");
	test_append(&mms);

	printf("\n---- INPUT REDIRECTIONS ----\n");
	test_input_redirections(&mms);

	printf("\n---- MULTIPLE REDIRECTIONS ----\n");
	test_multiple_redirections(&mms);

	printf("\n");
	printf("============================================================\n");
	printf("TOTAL TESTS : %d\n", g_test_number);
	printf("FAILED       : %d\n", g_failed);
	printf("PASSED       : %d\n", g_test_number - g_failed);
	printf("============================================================\n");

	free_test_mms(&mms);
	return (g_failed != 0);
}