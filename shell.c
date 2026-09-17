#include "shell.h"
/**
 * main - Simple UNIX command line interpreter
 *
 * Return: Always 0
 */
extern char **environ;

int main(void)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	pid_t child;
	int status;
	char *argv[2];

	while (1)
	{
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "$ ", 2);

		read = getline(&line, &len, stdin);

		if (read == -1)
		{
			free(line);
			if (isatty(STDIN_FILENO))
				write(STDOUT_FILENO, "\n", 1);
			return (0);
		}

		line[read - 1] = '\0';

		if (line[0] == '\0')
			continue;

		argv[0] = line;
		argv[1] = NULL;

		child = fork();

		if (child == -1)
		{
			perror("fork");
			continue;
		}

		if (child == 0)
		{
			if (execve(argv[0], argv, environ) == -1)
			{
				perror("./simple_shell");
				exit(127);
			}
		}
		else
		{
			wait(&status);
		}
	}
	free(line);
	return (0);
}
