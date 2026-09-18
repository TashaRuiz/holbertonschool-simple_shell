#include "shell.h"
/**
 * main - Simple UNIX command line interpreter
 *
 * Return: Always 0
 */
int main(void)
{
	char **args;
	char *input = NULL;
	size_t buffer_size = 0;
	int count = 0, mode = isatty(STDIN_FILENO), comparator;

	while (1)
	{
		if (mode)
			printf("#cisfun$ ");
		if (getline(&input, &buffer_size, stdin) == -1) /* Reading the user input */
		{
			free(input);
			break; /* Handling the eof (ctrl + D) */
		}
		count++;
		if (input[strlen(input)] == '\n') /* Trim trailing the newline */
			input[strlen(input)] = '\0';
		args = parse_input(input); /* Tokenize the user input */

		if (args == NULL)
		{
			free(input);
			free(args);
			continue;
		}
		if (args[0] != NULL)
		{
			comparator = env_fetch(args, input, count);
			free(args);
			if (comparator == 0)
				continue;
			else
				break;
		}
		free(args);
	}
	if (mode)
		printf("\n");
	return (0);
}
