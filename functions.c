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
	char *start;
	char *end;
	int count = 0;

	start = line;

	while (*start != '\0' && count < 63)
	{
		if (*start == '\0')
			break;

		while (*start == ' ' || *start == '\t')
			start++;

		end = start;

		while (*end != '\0' && *end != ' ' && *end != '\t')
			end++;

		if (*end != '\0')
		{
			*end = '\0';
			end++;
		}

		args[count] = start;
		count++;

		start = end;
	}
	args[count] = NULL;
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
/**
 * string_duplicate - duplicates a string
 * @str: string to duplicate
 *
 * Return: pointer to duplicated string, or NULL
 */
char *string_duplicate(char *str)
{
	char *copy;
	int i;
	int length;

	if (str == NULL)
		return (NULL);

	length = 0;
	while (str[length] != '\0')
		length++;

	copy = malloc(sizeof(char) * (length + 1));
	if (copy == NULL)
		return (NULL);

	for (i = 0; i <= length; i++)
		copy[i] = str[i];

	return (copy);
}
/**
 * find_character - finds a character in a string
 * @str: string to search
 * @character: character to find
 *
 * Return: pointer to character, or NULL
 */
char *find_character(char *str, char character)
{
	if (str == NULL)
		return (NULL);

	while (*str != '\0')
	{
		if (*str == character)
			return (str);

		str++;
	}

	return (NULL);
}/**
 * string_compare - compares two strings
 * @s1: first string
 * @s2: second string
 *
 * Return: 0 if strings are equal
 */
int string_compare(char *s1, char *s2)
{
	int i;

	if (s1 == NULL || s2 == NULL)
		return (-1);

	i = 0;

	while (s1[i] != '\0' && s2[i] != '\0')
	{
		if (s1[i] != s2[i])
			return (s1[i] - s2[i]);

		i++;
	}

	return (s1[i] - s2[i]);
}
/**
 * read_line - reads a line from standard input
 *
 * Return: allocated line, or NULL on EOF/error
 */
char *read_line(void)
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
		bytes = read(STDIN_FILENO, &character, 1);

		if (bytes == 0)
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
