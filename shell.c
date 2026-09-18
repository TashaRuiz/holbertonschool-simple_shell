#include "shell.h"
/**
 * main - Simple UNIX command line interpreter
 *
 * Return: Always 0
 */
while (getline(...) != -1)
{
	/* remove newline */
	/* tokenize */
	/* fork */

	if (fork() == 0)
	{
		execve(args[0], args, environ);
		perror(args[0]);
		exit(127);
	}

	wait(...);
}
