#include "shell.h"
/**
 * get_path - gets PATH from environment
 * @env: environment variables
 *
 * Return: PATH value or NULL
 */
char *get_path(char **env)
{
	int i;

	if (env == NULL)
		return (NULL);

	for (i = 0; env[i] != NULL; i++)
	{
		if (strncmp(env[i], "PATH=", 5) == 0)
			return (env[i] + 5);
	}
	return (NULL);
}
/**
 * find_command - finds a command in PATH
 * @command: command to find
 * @env: environment variables
 *
 * Return: full path to command or NULL
 */
char *find_command(char *command, char **env)
{
	char *path;
	char *start;
	char *end;
	char *full;
	char *dir;
	int length;
	int i;

	if (command == NULL)
		return (NULL);

	if (find_character(command, '/') != NULL)
	{
		if (access(command, X_OK) == 0)
			return (string_duplicate(command));
		return (NULL);
	}

	path = get_path(env);
	if (path == NULL || *path == '\0')
		return (NULL);

	start = path;
	while (1)
	{
		end = start;
		while (*end != ':' && *end != '\0')
			end++;

		length = end - start;

		if (length > 0)
		{
			dir = malloc(length + 1);
			if (dir == NULL)
				return (NULL);

			for (i = 0; i < length; i++)
				dir[i] = start[i];
			dir[length] = '\0';

			full = build_path(dir, command);
			free(dir);

			if (full != NULL)
				return (full);
		}
		if (*end == '\0')
			break;
		start = end + 1;
	}
	return (NULL);
}
/**
 * execute_command - executes a command
 * @args: command arguments
 * @env: environment variables
 * @program: program name
 *
 * Return: exit status of command
 */
int execute_command(char **args, char **env, char *program)
{
	char *command;
	pid_t pid;
	int status;

	command = find_command(args[0], env);

	if (command == NULL)
	{
		fprintf(stderr, "%s: 1: %s: not found\n",
				program, args[0]);
		return (127);
	}

	pid = fork();

	if (pid == -1)
	{
		free(command);
		return (1);
	}
	if (pid == 0)
	{
		execve(command, args, env);
		free(command);
		exit(127);
	}

	waitpid(pid, &status, 0);
	free(command);

	if (WIFEXITED(status))
		return (WEXITSTATUS(status));

	return (1);
}
/**
 * process_line - processes one command line
 * @line: command line
 * @env: environment variables
 * @program: program name
 *
 * Return: command status, or -1 to exit shell
 */
int process_line(char *line, char ***env, char *program, int *exit_shell, int last_status)
{
	char *command;
	/*char *saveptr;*/
	int status = 0;
	char *line_copy;

	remove_comments(line); /*Call it before split on ; or do anything else with the line*/
	/* Work on a copy so we do not destroy the original line */
	line_copy = string_duplicate(line);
	if (line_copy == NULL)
		return (1);

	command = strtok(line_copy, ";");
	while (command != NULL)
	{
		/* Skip leading spaces/tabs */
		while (*command == ' ' || *command == '\t')
			command++;

		if (*command != '\0')
		{
			status = execute_logical_list(command, env, program, exit_shell, last_status);

			/* If the user typed "exit", stop processing more commands */
			if (*exit_shell)
				break;
		}
		command = strtok(NULL, ";");
	}
	free(line_copy);
	return (status);
}
/**
 * read_line_from_fd - reads one line from a file descriptor
 * @fd: file descriptor
 *
 * Return: allocated line (without newline), or NULL on EOF/error
 */
char *read_line_from_fd(int fd)
{
	char *line;
	char character;
	int i;
	ssize_t bytes;

	line = malloc(1024);
	if (line == NULL)
		return (NULL);

	i = 0;
	while (i < 1023)
	{
		bytes = read(fd, &character, 1);
		if (bytes == 0)			/* EOF */
		{
			if (i == 0)
			{
				free(line);
				return (NULL);
			}
			break;
		}
		if (bytes == -1)
		{
			free(line);
			return (NULL);
		}
		if (character == '\n')
			break;

		line[i] = character;
		i++;
	}
	line[i] = '\0';
	return (line);
}
/**
 * main - Simple UNIX command line interpreter
 * @argc: number of arguments
 * @argv: array of arguments
 * @env: environment variables
 *
 * Return: status last command
 */
int main(int argc, char **argv, char **envp)
{
	char *line;
	char **env;
	int status = 0;
	int exit_shell = 0;
	int fd = -1; /* file descriptor instead of FILE* */
	char *program = argv[0];

	env = copy_environment(envp); /* or however you copy the env */

	if (env == NULL)
		return (1);
	/* ---------- Non-interactive mode: a file was given ---------- */
	if (argc >= 2)
	{
		fd = open(argv[1], O_RDONLY);
		if (fd == -1)
		{
			/* exact message required by the checker */
			fprintf(stderr, "%s: 0: Can't open %s\n", program, argv[1]);
			/* or if fprintf is also forbidden:
			   write the message with write() */
			free_environment(env);
			free_aliases();
			return (127);
		}
	}
	/* ---------- Main loop ---------- */
	while (!exit_shell)
	{
		if (fd == -1 && isatty(STDIN_FILENO))   /* solo si es terminal */
			write(STDOUT_FILENO, "$ ", 2);

		if (fd != -1)
		{
			/* read one line from the file descriptor */
			line = read_line_from_fd(fd); /* see helper below */
			/*EOF or error*/
			/*if (line == NULL)
				break;*/
		}
		else
		{
			line = read_line(); /* your existing interactive reader */
			/*if (line == NULL)
				break;*/
		}
		if (line == NULL)
			break;

		status = process_line(line, &env, program, &exit_shell, status);
		free(line);
	}
	if (fd != -1)
		close(fd);

	free_environment(env);
	free_aliases();
	return (status);
}
