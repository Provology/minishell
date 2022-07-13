#include "../includes/minishell.h"

t_global g_var;

void	test_print_env(char **temp)
{
	int i = -1;
	while (*temp)
	{
		printf("%d) %s\n", ++i, *temp);
		temp++;
	}
}

int	mini_err(t_store *st, int err)
{
	printf("errno: %d\n", errno);
	printf("Err occured: %d\n", err);
	if (err == 0)
	{
		free(st->par);
	}
	exit(1);
}

void	*mini_calloc(size_t nmemb, size_t size, t_store *st)
{
	char	*v;

	if (size && nmemb > (__SIZE_MAX__ / size))
		mini_err(st, ERR_MALLOC0);
	v = malloc(nmemb * size);
	if (!v)
		mini_err(st, ERR_MALLOC0);
	else
		return (ft_memset(v, '\0', (nmemb * size)));
	return (NULL);
}

int	get_cmd_size(t_cmd *cmds)
{
	int i;

	i = 0;
	while (cmds[i].empty == 0)
		i++;
	return (i);
}

int	get_list_size(t_list *list)
{
	int	i;

	i = 0;
	while (list)
	{
		list = list->next;
		i++;
	}
	return (i);
}

int	get_void_size(void **arr)
{
	int	i;

	i = 0;
	while (arr[i] != NULL)
		i++;
	return (i);
}

char	*get_path_str(char **env)
{
	if (!env)
		return (NULL);
	while (ft_strncmp(*env, "PATH=", 5))
	{
		if (!*env)
			return (NULL);
		env++;
	}
	return (*env + 5);
}

int	startup(t_store *st, char **env)
{
	int	i;

	env_to_list(env);
	g_var.last_exec = 0;
	st->env = env;
	st->par = NULL;
	return (0);
}

void	malloc_appropriate_struct(t_store *st, t_cmd *cmds)
/* That func have inner protection. It's leaks free! */
{
	int	i;

	st->par = mini_calloc(st->size + 1, sizeof(void *), st);
	st->pip = mini_calloc(st->size, sizeof(void *), st);
	i = -1;
	while (++i < st->size)
	{
		st->pip[i] = mini_calloc(2, sizeof(int), st);
		st->par[i] = mini_calloc(get_list_size(cmds->command) + 1, sizeof(void *), st);
		cmds++;
	}
	i = -1;
	while (++i < st->size - 1)
	{
		if (pipe(st->pip[i]) == -1)
			mini_err(st, ERR_MALLOC0);
	}
	return ;
}

void	create_appropriate_struct(t_store *st, t_cmd *cmds)
{
	int		i;
	int		e;
	t_list	*curlist;

	i = -1;
	while (++i < st->size)
	{
		if (!cmds->command)
			continue ;
		e = 0;
		curlist = (cmds + i)->command;
		while (curlist)
		{
			st->par[i][e] = ((t_file *)curlist->content)->name;
			//printf("%d) st->par = %s\n", i, st->par[i][e]);
			curlist = curlist->next;
			e++;
		}
		st->par[i][e] = NULL;
	}
}

char	*strjoin_char(char *s1, char *s2, char delim)
{
	char	*out;
	int		size1;
	int		size2;
	int		i;

	size1 = ft_strlen(s1);
	size2 = ft_strlen(s2);
	out = malloc((size1 + size2 + 2) * sizeof(char));
	if (!out)
		return (NULL);
	i = -1;
	while (++i < size1)
		out[i] = s1[i];
	out[i++] = delim;
	size1 = -1;
	while (++size1 <= size2)
		out[i++] = s2[size1];
	return (out);
}

int	 get_infile_fd(t_store *st, t_cmd *cmds, int num)
{
	t_list	*lst;
	int		temp_fd;
	int		fd_out;

	if (cmds->infiles == NULL)
	{
		if (num == 0)
			return (0);
		close(st->pip[num - 1][1]);
		if (dup2(st->pip[num - 1][0], 0) == -1)
			mini_err(st, ERR_SUB_PRCCESS);
		return (0);
	}
	lst = cmds->infiles;
	while (lst->next)
	{
		temp_fd = open(((t_file *)lst->content)->name, O_WRONLY | O_TRUNC | O_CREAT, 0664);
		if (temp_fd == -1)
			mini_err(st, ERR_SUB_PRCCESS);
		lst = lst->next;
	}
	if (((t_file *)lst->content)->append == 1)
		temp_fd = open(((t_file *)lst->content)->name, O_RDONLY);
	if (temp_fd == -1)
		mini_err(st, ERR_SUB_PRCCESS);
	if (dup2(temp_fd, 0) == -1)
		mini_err(st, ERR_SUB_PRCCESS);
	return (0);
}

int	get_outfile_fd(t_store *st, t_cmd *cmds, int num)
{
	t_list	*lst;
	int		temp_fd;

	if (cmds->outfiles == NULL)
	{
		if (num == st->size - 1)
			return (0);
		if (dup2(st->pip[num][1], 1) == -1)
		{
			close(st->pip[num][1]);
			mini_err(st, ERR_SUB_PRCCESS);
		}
		close(st->pip[num][1]);
		return (0);
	}
	lst = cmds->outfiles;
	while (lst->next)
	{
		if (((t_file *)lst->content)->append == 0)
		{
			temp_fd = open(((t_file *)lst->content)->name, O_WRONLY | O_TRUNC | O_CREAT, 0664);
			if (temp_fd == -1)
				mini_err(st, ERR_SUB_PRCCESS);
			close(temp_fd);
		}
		lst = lst->next;
	}
	if (((t_file *)lst->content)->append == 1)
		temp_fd = open(((t_file *)lst->content)->name, O_WRONLY | O_CREAT | O_APPEND, 0664);
	else
		temp_fd = open(((t_file *)lst->content)->name, O_WRONLY | O_CREAT | O_TRUNC, 0664);
	if (temp_fd == -1)
		mini_err(st, ERR_SUB_PRCCESS);
	if (dup2(temp_fd, 1) == -1)
		mini_err(st, ERR_SUB_PRCCESS);
	return (0);
}

int	find_file_by_dir(t_store *st, char **par, int e)
{
	int 	i;
	char	*str;

	i = -1;
	while (++i < st->path_size)
	{
		str = strjoin_char(st->path[i], par[0], '/');
		if (!str)
			mini_err(st, 0);
		// printf("!!!%s\n", str);
		if (access(str, F_OK) == 0)
		{
			free(par[0]);
			par[0] = str;
			return (1);
		}
		free(str);
	}
	return (0);
}

int	is_command_ok(t_store *st)
{
	int	i;

	i = -1;
	while (++i < st->size)
	{
		if (built_in_check(st->par[i][0]))
			continue ;
		if (find_file_by_dir(st, st->par[i], i) == 0)
		{
			printf("minishell: %s: command not found\n", st->par[i][0]);
			return (0);
		}
	}
	return (1);
}

int	pipe_exec_subfunc(t_store *st, t_cmd *cmds, int num)
{
	int	status;

	get_infile_fd(st, cmds, num);
	get_outfile_fd(st, cmds, num);
	if(!is_built_in(cmds[num].command)) // built-in in progress
		exit(0);
	st->last_result = execve(st->par[num][0], st->par[num], st->env);
	if (st->last_result == -1)
		mini_err(st, ERR_FOR_SUBFUNC);
	exit(0);
	return (0);
}

void test_func(t_store *st, t_cmd *cmds, int num)
{
	printf("fork start :%d\n", num);
	printf("%d) command: %s\n", num, st->par[num][0]);
	int i = 1;
	while (st->par[num][i])
	{
		printf("%d) arg[%d]: %s\n", num, i, st->par[num][i]);
		i++;
	}
}

int	pipe_exec(t_store *st, t_cmd *cmds, int num)
{
	int	pid;
	int	status;

	// test_func(st, cmds, num);
	pid = fork();
	if (pid < 0)
		mini_err(st, ERR_FORK_INIT);
	if (pid == 0)
	{
		pipe_exec_subfunc(st, cmds, num);
	}
	return (pid);
}

int	main_loop(t_store *st, t_cmd *cmds)
{
	int	i;
	int	e;
	int	pid;
	int	status;

	malloc_appropriate_struct(st, cmds);
	create_appropriate_struct(st, cmds);
	i = -1;
	if (!is_command_ok(st))
		return (0);
	while (++i < st->size)
		pid = pipe_exec(st, cmds, i);
	i = -1;
	while (++i < st->size - 1)
	{
		e = -1;
		while (++e < 2)
			close(st->pip[i][e]);
	}
	waitpid(pid, &status, 0);
	g_var.last_exec = WEXITSTATUS(status);
	return (0);
}

int	main(int args, char **argv, char **env)
{
	t_store	st;
	int		err;
	t_cmd	*cmds;
	char	*str;

	if (startup(&st, env))
		return (0);
	g_var.store = &st;
	while (1)
	{
		str = rl_gets();
		if (!str || !*str)
			continue;
		cmds = parser(str, &err);
		if (err)
			continue;
		st.env = list_to_env();
		st.path = ft_split(get_var("PATH"), ':');
		if (!st.path || !st.env)
			mini_err(&st, 0);
		st.path_size = get_void_size((void *)st.path);
		st.size = get_cmd_size(cmds);
		err = main_loop(&st, cmds);
	}
}
