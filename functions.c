#include "shell.h"
/**
 * build_path - builds and checks a command path
 * @dir: directory from PATH
 * @command: command to find
 *
 * Return: full path if found, otherwise NULL
 */
char *build_path(char *dir, char *command)
{
	char *full;
	size_t size;

	size = strlen(dir) + strlen(command) + 2;
	full = malloc(size);
	if (full == NULL)
		return (NULL);

	sprintf(full, "%s/%s", dir, command);

	if (access(full, X_OK) == 0)
		return (full);

	free(full);
	return (NULL);
}
/**
 * split_line - splits input into arguments
 * @line: input line
 * @args: array where arguments are stored
 */
void split_line(char *line, char **args)
{
	char *token;
	int i = 0;

	token = strtok(line, " \t");
	while (token != NULL && i < 63)
	{
		args[i] = token;
		i++;
		token = strtok(NULL, " \t");
	}
	args[i] = NULL;
}
/**
 * print_env - prints the environment
 * @env: environment variables
 */
void print_env(char **env)
{
	int i;

	for (i = 0; env[i] != NULL; i++)
	{
		write(STDOUT_FILENO, env[i], strlen(env[i]));
		write(STDOUT_FILENO, "\n", 1);
	}
}
