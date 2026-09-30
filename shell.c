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
	int length;

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
	/*while (*start != '\0')*/
	while (1)
	{
		end = start;
		/*if (*end == '\0')
			break;

		start = end + 1;*/
		while (*end != ':' && *end != '\0')
		{
			end++;
		}
		length = end - start;
		if (length > 0)
		{
			full = build_path(start, command);
			if (full != NULL)
				return (full);
		}
		/*length = end - start;*/
		if (*end == '\0')
		{
			 break;
		}
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
 * read_line_from_file - reads one line from a FILE*
 * @fp: the file pointer
 *
 * Return: allocated line (without the newline), or NULL on EOF/error
 */
char *read_line_from_file(FILE *fp)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;

	nread = getline(&line, &len, fp);
	if (nread == -1)
	{
		free(line);
		return (NULL);
	}
	/* remove the trailing newline if present */
	if (nread > 0 && line[nread - 1] == '\n')
		line[nread - 1] = '\0';

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
int main(int argc, char **argv, char **env)
{
	char *line;
	int status = 0;
	int exit_shell = 0;
	FILE *script = NULL;
	char *program = argv[0];

	env = copy_environment(environ);	/* or however you copy the env */

	/* ---------- Non-interactive mode: a file was given ---------- */
	if (argc >= 2)
	{
		script = fopen(argv[1], "r");
		if (script == NULL)
		{
			fprintf(stderr, "%s: 0: Can't open %s\n", program, argv[1]);
			free_environment(env);
			free_aliases(); /* if you have it */
			return (127);
		}
	}
	/* ---------- Main loop ---------- */
	while (!exit_shell)
	{
		if (script != NULL)
		{
			/* read from the script file */
			line = read_line_from_file(script); /* your function to read a line from FILE* */;
			if (line == NULL)	/* EOF */
				break;
		}
		else
		{
			/* interactive mode – read from stdin */
			line = read_line();
			if (line == NULL)	/* Ctrl+D / EOF */
				break;
		}

		status = process_line(line, &env, program, &exit_shell, status);
		free(line);
	}
	if (script != NULL)
		fclose(script);

	free_environment(env);
	free_aliases();
	return (status);
}
