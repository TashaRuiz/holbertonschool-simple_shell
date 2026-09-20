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
	char *copy;
	char *dir;
	char *full;

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

	copy = string_duplicate(path);
	if (copy == NULL)
		return (NULL);

	dir = strtok(copy, ":");

	while (dir != NULL)
	{
		full = build_path(dir, command);

		if (full != NULL)
		{
			free(copy);
			return (full);
		}
		dir = strtok(NULL, ":");
	}
	free(copy);
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
int process_line(char *line, char **env, char *program)
{
	char *args[64];

	split_line(line, args);

	if (args[0] == NULL)
		return (0);

	if (string_compare(args[0], "exit") == 0)
		return (-1);

	if (string_compare(args[0], "env") == 0)
	{
		print_env(env);
		return (0);
	}
	return (execute_command(args, env, program));
}
/**
 * main - Simple UNIX command line interpreter
 * @argc: number of arguments
 * @argv: array of arguments
 * @env: environment variables
 *
 * Return: status last command
 */
int main(int argc, char **argv, char **env)
{
	char *line;
	int status = 0;
	int result;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "$ ", 2);

		line = read_line();

		if (line == NULL)
		{
			if (isatty(STDIN_FILENO))
				write(STDOUT_FILENO, "\n", 1);

			break;
		}

		result = process_line(line, env, argv[0]);
		free(line);

		if (result == -1)
			break;

		status = result;
	}
	return (status);
}
