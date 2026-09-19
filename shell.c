#include "shell.h"
/**
 * main - Simple UNIX command line interpreter
 *
 * Return: Always 0
 */
int main(void)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	pid_t child;
	int status;
	/*char *argv[2];*/
	char *argv[64];
	char *token;
	char *start;
	char *end;
	int argc = 0;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "$ ", 4);
	
		read = getline(&line, &len, stdin);
	
		if (read == -1)
		{
			if (isatty(STDIN_FILENO))
				write(STDOUT_FILENO, "\n", 1);
			/*free(line);*/
			return (0);
		}

		if (read > 0 && line[read - 1] == '\n')
			line[read - 1] = '\0';
		/*if(line[0] == '\0')
			continue;

		argv[0] = strtok(line, "\t");
		argv[1] = NULL;
	
		if (argv[0] == NULL)
			continue;*/
		token = strtok(line, " \t");
		while (token != NULL && argc < 63)
		{
			argv[argc] = token;
			argc++;
			token = strtok(NULL, " \t");
		}
		argv[argc] = NULL;

		if (argc == 0)
			continue;

		start = line;/*the begining of the program to avoid whitespace*/
		while (*start == ' ' || *start == '\t')
			start++;
		end = start + strlen(start) - 1;
		while (end >= start && (*end == ' ' || *end == '\t'))
		{
			*end = '\0';
			end--;
		}
		argv[0] = start;
		argv[1] = NULL;/*the end of whitespace program*/
		
		child = fork();
		if (child == -1)
		{
			perror("./hsh");
			free(line);
			return (1);
		}

		if (child == 0)
		{
			execve(argv[0], argv, environ);
			perror(argv[0]);
			exit(1);
		}
		/*start = line;
		while (*start == ' ' || *start == '\t')
			start++;
		
		end = start + strlen(start) - 1;
		while (end >= start && (*end == ' ' || *end == '\t'))
		{
			*end = '\0';
			end--;
		}
		argv[0] = start;
		argv[1] = NULL;*/
		wait(&status);
	}
	free(line);
	return (status);
}
