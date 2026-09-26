#include "shell.h"
/**
 * update_directory_vars - updates PWD and OLDPWD
 * @env: environment variables
 * @oldpwd: previous directory
 * @newpwd: new directory
 *
 * Return: 0 on success, 1 on failure
 */
int update_directory_vars(char **env, char *oldpwd, char *newpwd)
{
	int i;
	int old_len;
	int new_len;
	char *new_value;

	if (oldpwd == NULL || newpwd == NULL)
		return (1);

	old_len = string_length(oldpwd);
	new_len = string_length(newpwd);

	for (i = 0; env[i] != NULL; i++)
	{
		if (string_starts_with(env[i], "OLDPWD="))
		{
			new_value = malloc(old_len + 8);

			if (new_value == NULL)
				return (1);

			sprintf(new_value, "OLDPWD=%s", oldpwd);
			free(env[i]);
			env[i] = new_value;
		}
		else if (string_starts_with(env[i], "PWD="))
		{
			new_value = malloc(new_len + 5);

			if (new_value == NULL)
				return (1);

			sprintf(new_value, "PWD=%s", newpwd);
			free(env[i]);
			env[i] = new_value;
		}
	}
	return (0);
}
/**
 * handle_cd - changes the current working directory
 * @args: command arguments
 * @env: environment variables
 *
 * Return: 0 on success, 1 on failure
 */
int handle_cd(char **args, char **env)
{
	char *home;
	char *oldpwd;
	char *pwd;
	char *target;
	char *old_directory;
	int i;

	home = NULL;
	oldpwd = NULL;
	pwd = NULL;
	target = NULL;
	old_directory = NULL;

	for (i = 0; env[i] != NULL; i++)
	{
		if (string_starts_with(env[i], "HOME="))
			home = env[i] + 5;
		else if (string_starts_with(env[i], "OLDPWD="))
			oldpwd = env[i] + 7;
		else if (string_starts_with(env[i], "PWD="))
			pwd = env[i] + 4;
	}
	/* cd */
	if (args[1] == NULL)
	{
		if (home == NULL)
			return (1);
		target = string_duplicate(home);
	}
	/* cd - */
	else if (string_compare(args[1], "-") == 0)
	{
		if (oldpwd == NULL)
		{
			if (pwd != NULL)
			{
				write(STDOUT_FILENO, pwd, string_length(pwd));
				write(STDOUT_FILENO, "\n", 1);
			}
			return (0);
		}
		target = string_duplicate(oldpwd);
	}
	/* cd DIRECTORY */
	else
	{
		target = string_duplicate(args[1]);
	}
	if (target == NULL)
		return (1);
	/*
	 * Save PWD before changing directory.
	 */
	if (pwd != NULL)
	{
		old_directory = string_duplicate(pwd);

		if (old_directory == NULL)
		{
			free(target);
			return (1);
		}
	}
	/*
	 * Change directory.
	 */
	if (chdir(target) == -1)
	{
		fprintf(stderr, "./hsh: 1: cd: can't cd to %s\n", target);
		free(old_directory);
		free(target);
		return (1);
	}
	/*
	 * Update environment.
	 */
	if (update_directory_vars(env, old_directory, target) != 0)
	{
		free(old_directory);
		free(target);
		return (1);
	}
	/*
	 * cd - prints the directory changed to.
	 */
	if (args[1] != NULL && string_compare(args[1], "-") == 0)
	{
		write(STDOUT_FILENO, target, string_length(target));
		write(STDOUT_FILENO, "\n", 1);
	}
	free(old_directory);
	free(target);

	return (0);
}
/**
 * handle_setenv - handles the setenv builtin
 * @args: command arguments
 * @env: pointer to environment
 *
 * Return: 0 on success, 1 on error
 */
int handle_setenv(char **args, char ***env)
{
	char *new_variable;
	char *old_variable;
	char *name;
	char *value;
	int name_len;
	int value_len;
	int i;
	int j;

	if (args[1] == NULL)
		return (1);

	name = args[1];
	value = args[2];

	if (value == NULL)
		return (0);

	name_len = string_length(name);
	value_len = string_length(value);

	new_variable = malloc(name_len + value_len + 2);

	if (new_variable == NULL)
		return (1);

	for (i = 0; i < name_len; i++)
		new_variable[i] = name[i];

	new_variable[name_len] = '=';

	for (j = 0; j < value_len; j++)
		new_variable[name_len + 1 + j] = value[j];

	new_variable[name_len + value_len + 1] = '\0';

	for (i = 0; (*env)[i] != NULL; i++)
	{
		if (string_starts_with((*env)[i], name)
			&& (*env)[i][name_len] == '=')
		{
			old_variable = (*env)[i];
			(*env)[i] = new_variable;
			free(old_variable);
			return (0);
		}
	}
	i = add_environment(new_variable, env);
	free(new_variable);
	return (i);
}
/**
 * handle_unsetenv - removes an environment variable
 * @args: command arguments
 * @env: pointer to environment
 *
 * Return: 0 on success, 1 on failure
 */
int handle_unsetenv(char **args, char ***env)
{
	int i, j;
	int name_len;

	if (args[1] == NULL)
		return (1);

	name_len = string_length(args[1]);

	for (i = 0; (*env)[i] != NULL; i++)
	{
		if (string_starts_with((*env)[i], args[1]) &&
		    (*env)[i][name_len] == '=')
		{
			free((*env)[i]);
			/* shift the remaining variables down */
			for (j = i; (*env)[j] != NULL; j++)
				(*env)[j] = (*env)[j + 1];
			return (0);
		}
	}
	return (0); /* variable not found is usually not an error */
}
/**
 * string_length - gets the length of a string
 * @str: string to measure
 *
 * Return: length of string
 */
int string_length(char *str)
{
	int length;

	if (str == NULL)
		return (0);

	length = 0;

	while (str[length] != '\0')
		length++;

	return (length);
}
/**
 * string_starts_with - checks if a string starts with a prefix
 * @str: string to check
 * @prefix: prefix to find
 *
 * Return: 1 if prefix matches, 0 otherwise
 */
int string_starts_with(char *str, char *prefix)
{
	int i;

	if (str == NULL || prefix == NULL)
		return (0);

	i = 0;

	while (prefix[i] != '\0')
	{
		if (str[i] != prefix[i])
			return (0);

		i++;
	}
	return (1);
}
/**
 * copy_environment - makes a copy of the environment
 * @env: original environment
 *
 * Return: copied environment, or NULL
 */
char **copy_environment(char **env)
{
	char **copy;
	int count;
	int i;

	if (env == NULL)
		return (NULL);

	count = 0;

	while (env[count] != NULL)
		count++;

	copy = malloc(sizeof(char *) * (count + 1));

	if (copy == NULL)
		return (NULL);

	for (i = 0; i < count; i++)
	{
		copy[i] = string_duplicate(env[i]);

		if (copy[i] == NULL)
		{
			while (i > 0)
			{
				i--;
				free(copy[i]);
			}
			free(copy);
			return (NULL);
		}
	}
	copy[count] = NULL;
	return (copy);
}
/**
 * free_environment - frees a copied environment
 * @env: environment to free
 */
void free_environment(char **env)
{
	int i;

	if (env == NULL)
		return;

	for (i = 0; env[i] != NULL; i++)
		free(env[i]);

	free(env);
}
/**
 * add_environment - adds a variable to the environment
 * @variable: variable to add
 * @env: pointer to environment
 *
 * Return: 0 on success, 1 on failure
 */
int add_environment(char *variable, char ***env)
{
	char **new_env;
	int count;
	int i;

	count = 0;

	while ((*env)[count] != NULL)
		count++;

	new_env = malloc(sizeof(char *) * (count + 2));

	if (new_env == NULL)
		return (1);

	for (i = 0; i < count; i++)
		new_env[i] = (*env)[i];

	new_env[count] = string_duplicate(variable);

	if (new_env[count] == NULL)
	{
		free(new_env);
		return (1);
	}
	new_env[count + 1] = NULL;

	free(*env);

	*env = new_env;

	return (0);
}
/**
 * string_to_int - converts a string to an integer
 * @str: string containing a number
 *
 * Return: converted integer
 */
int string_to_int(char *str)
{
	int number = 0;
	int sign = 1;

	if (str == NULL)
		return (0);

	if (*str == '-')
	{
		sign = -1;
		str++;
	}
	while (*str >= '0' && *str <= '9')
	{
		number = number * 10 + (*str - '0');
		str++;
	}
	return (number * sign);
}
/**
 * is_number - checks if a string contains only digits
 * @str: string to check
 *
 * Return: 1 if number, 0 otherwise
 */
int is_number(char *str)
{
	int i;

	if (str == NULL || *str == '\0')
		return (0);
	for (i = 0; str[i] != '\0'; i++)
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
	}
	return (1);
}
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
		while (*start == ' ' || *start == '\t')
			start++;

		if (*start == '\0')
			break;

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
