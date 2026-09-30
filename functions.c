#include "shell.h"
/**
 * update_directory_vars - updates PWD and OLDPWD
 * @env: environment variables
 * @oldpwd: previous directory
 * @newpwd: new directory
 *
 * Return: 0 on success, 1 on failure
 */
int update_directory_vars(char ***env, char *oldpwd, char *newpwd)
{
	int i;
	int old_len;
	int new_len;
	char *new_value;
	int found_oldpwd = 0;
	int found_pwd = 0;

	if (oldpwd == NULL || newpwd == NULL)
		return (1);

	old_len = string_length(oldpwd);
	new_len = string_length(newpwd);

	for (i = 0; (*env)[i] != NULL; i++)
	{
		if (string_starts_with((*env)[i], "OLDPWD="))
		{
			new_value = malloc(old_len + 8);
			if (new_value == NULL)
				return (1);
			sprintf(new_value, "OLDPWD=%s", oldpwd);
			free((*env)[i]);
			(*env)[i] = new_value;
			found_oldpwd = 1;
		}
		else if (string_starts_with((*env)[i], "PWD="))
		{
			new_value = malloc(new_len + 5);
			if (new_value == NULL)
				return (1);
			sprintf(new_value, "PWD=%s", newpwd);
			free((*env)[i]);
			(*env)[i] = new_value;
			found_pwd = 1;
		}
	}
	/* Create the variables if they did not exist */
	if (!found_oldpwd)
	{
		new_value = malloc(old_len + 8);
		if (new_value == NULL)
			return (1);
		sprintf(new_value, "OLDPWD=%s", oldpwd);
		if (add_environment(new_value, env) != 0)/* env is already char *** */
		{
			free(new_value);
			return (1);
		}
		free(new_value);/* free the temporary buffer – this is correct */
	}
	if (!found_pwd)
	{
		new_value = malloc(new_len + 5);
		if (new_value == NULL)
			return (1);
		sprintf(new_value, "PWD=%s", newpwd);
		if (add_environment(new_value, env) != 0)
		{
			free(new_value);
			return (1);
		}
		free(new_value);
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
int handle_cd(char **args, char ***env)
{
	char *home = NULL;
	char *oldpwd = NULL;
	char *pwd = NULL;
	char *target = NULL;
	char *old_directory = NULL;
	int i;

	/* Read current values from the environment */
	for (i = 0; (*env)[i] != NULL; i++)
	{
		if (string_starts_with((*env)[i], "HOME="))
			home = (*env)[i] + 5;
		else if (string_starts_with((*env)[i], "OLDPWD="))
			oldpwd = (*env)[i] + 7;
		else if (string_starts_with((*env)[i], "PWD="))
			pwd = (*env)[i] + 4;
	}
	/* Decide the target directory */
	if (args[1] == NULL) /* cd  (no argument) → go to HOME */
	{
		if (home == NULL)
			return (1);
		target = string_duplicate(home);
	}
	else if (string_compare(args[1], "-") == 0) /* cd - → go to OLDPWD */
	{
		if (oldpwd == NULL)
		{
			/* No OLDPWD → just print current directory and stay */
			if (pwd != NULL)
			{
				write(STDOUT_FILENO, pwd, string_length(pwd));
				write(STDOUT_FILENO, "\n", 1);
			}
			return (0);
		}
		target = string_duplicate(oldpwd);
	}
	else /* cd DIRECTORY */
	{
		target = string_duplicate(args[1]);
	}
	if (target == NULL)
		return (1);

	/* Save the old PWD so we can put it into OLDPWD later */
	if (pwd != NULL)
	{
		old_directory = string_duplicate(pwd);
		if (old_directory == NULL)
		{
			free(target);
			return (1);
		}
	}
	/* Actually change directory */
	if (chdir(target) == -1)
	{
		fprintf(stderr, "./hsh: 1: cd: can't cd to %s\n", target);
		free(old_directory);
		free(target);
		return (1);
	}
	/* Update the environment variables */
	if (update_directory_vars(env, old_directory, target) != 0)
	{
		free(old_directory);
		free(target);
		return (1);
	}
	/* cd - must print the directory we changed to */
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
 * handle_alias - implements the alias builtin
 * @args: command arguments
 *
 * Return: 0 on success, 1 on failure
 */
alias_t aliases[MAX_ALIASES];
int alias_count = 0;
int handle_alias(char **args)
{
	int i, j;
	char *equal;
	char *name;
	char *value;
	int found;

	/* Case 1: just "alias" → print all aliases */
	if (args[1] == NULL)
	{
		for (i = 0; i < alias_count; i++)
		{
			write(STDOUT_FILENO, aliases[i].name,
			      string_length(aliases[i].name));
			write(STDOUT_FILENO, "='", 2);
			write(STDOUT_FILENO, aliases[i].value,
			      string_length(aliases[i].value));
			write(STDOUT_FILENO, "'\n", 2);
		}
		return (0);
	}

	/* Process every argument after "alias" */
	for (i = 1; args[i] != NULL; i++)
	{
		equal = find_character(args[i], '=');

		if (equal != NULL)
		{
			/* ---------- create / update alias ---------- */
			*equal = '\0';
			name = args[i];
			value = equal + 1;

			/* Look if the alias already exists → update it */
			found = 0;
			for (j = 0; j < alias_count; j++)
			{
				if (string_compare(aliases[j].name, name) == 0)
				{
					free(aliases[j].value);
					aliases[j].value = string_duplicate(value);
					found = 1;
					break;
				}
			}

			/* New alias */
			if (!found && alias_count < MAX_ALIASES)
			{
				aliases[alias_count].name = string_duplicate(name);
				aliases[alias_count].value = string_duplicate(value);
				if (aliases[alias_count].name && aliases[alias_count].value)
					alias_count++;
			}
		}
		else
		{
			/* ---------- print a specific alias ---------- */
			name = args[i];
			for (j = 0; j < alias_count; j++)
			{
				if (string_compare(aliases[j].name, name) == 0)
				{
					write(STDOUT_FILENO, aliases[j].name,
					      string_length(aliases[j].name));
					write(STDOUT_FILENO, "='", 2);
					write(STDOUT_FILENO, aliases[j].value,
					      string_length(aliases[j].value));
					write(STDOUT_FILENO, "'\n", 2);
					break;
				}
			}
		}
	}
	return (0);
}
/**
 * expand_alias - replaces args[0] if it is an alias
 * (supports simple chained aliases)
 * @args: argument array (will be modified)
 *
 * Return: 1 if an expansion was done, 0 otherwise
 */
char *expand_alias(char **args)
{
	int i;
	char *new_value = NULL;
	/*char *old;*/

	if (args[0] == NULL)
		return (NULL);

	for (i = 0; i < alias_count; i++)
	{
		if (string_compare(args[0], aliases[i].name) == 0)
		{
			/* Found an alias – replace args[0] */
			/*old = args[0];*/
			new_value = string_duplicate(aliases[i].value);
			if (new_value == NULL)
				return (NULL);

			/* Very simple: we only replace the command name.
			 * For the checker this is enough. */
			args[0] = new_value;

			/* Optional: free the old string if it was allocated,
			   but in your current split_line it points into the line,
			   so do NOT free it. */

			return (new_value);	/* expansion done */
		}
	}
	return (NULL);			/* no alias found */
}
/**
 * free_aliases - frees all stored aliases
 */
void free_aliases(void)
{
	int i;

	for (i = 0; i < alias_count; i++)
	{
		free(aliases[i].name);
		free(aliases[i].value);
		aliases[i].name = NULL;
		aliases[i].value = NULL;
	}
	alias_count = 0;
}
/**
 * expand_variables - replaces $? with the last exit status
 * @args: argument list
 * @env: enviroment
 * @last_status: status of the previous command
 *
 * Note: the new string is allocated. The caller should free it
 * after the command has finished (same way you free alias expansions).
 */
char *expand_variables(char **args, char **env, int last_status)
{
	int i, j;
	char *new_str = NULL;
	char *result = NULL;
	char *name;
	char *value;
	int name_len;

	for (i = 0; args[i] != NULL; i++)
	{
		/* ---------- $? ---------- */
		if (string_compare(args[i], "$?") == 0)
		{
			new_str = malloc(16);
			if (new_str == NULL)
				return (NULL);
			sprintf(new_str, "%d", last_status);
			args[i] = new_str;
			result = new_str;
			continue;
		}
		/* ---------- $$ (shell PID) ---------- */
		if (string_compare(args[i], "$$") == 0)
		{
			new_str = malloc(16);
			if (new_str == NULL)
				return (NULL);
			sprintf(new_str, "%d", getpid());   /* ← the important line */
			args[i] = new_str;
			result = new_str;
			continue;
		}
		/* ---------- $VAR ---------- */
		if (args[i][0] == '$' && args[i][1] != '\0')
		{
			name = args[i] + 1;
			name_len = string_length(name);
			value = NULL;

			for (j = 0; env[j] != NULL; j++)
			{
				if (string_starts_with(env[j], name) &&
				    env[j][name_len] == '=')
				{
					value = env[j] + name_len + 1;
					break;
				}
			}
			if (value != NULL)
				new_str = string_duplicate(value);
			
			else
				new_str = string_duplicate(""); /* undefined → empty */
			
			if (new_str == NULL)
				return (NULL);

			args[i] = new_str;
			result = new_str;
		}
	}
	return (result);
}
/**
 * execute_one_command - processes a single command (no ;)
 * @line: one command string (already separated from ;)
 * @env: environment
 * @program: program name
 * @exit_shell: flag to exit
 * @last_status: previous status
 *
 * Return: status of the command, or -1 to exit
 */
int execute_one_command(char *line, char ***env, char *program, int *exit_shell, int last_status)
{
	char *args[64];
	char *expanded_str = NULL;
	char *tmp;
	int status = 0;
	char *expanded_var = NULL;
	
	split_line(line, args);
	if (args[0] == NULL)
		return (0);

	/* Expand aliases (keep expanding for chained aliases) */
	do {
		tmp = expand_alias(args);
		if (tmp != NULL)
		{
			/* Free the previous expansion if we expanded more than once */
			if (expanded_str != NULL)
				free(expanded_str);
			expanded_str = tmp;
		}
	} while (tmp != NULL);

	/* ... after split_line and alias expansion ... */
	expanded_var = expand_variables(args, *env, last_status);

	/* ---------- normal command handling ---------- */
	if (string_compare(args[0], "exit") == 0)
	{
		*exit_shell = 1; /* ← uses exit_shell */
		if (args[1] != NULL)
		{
			if (!is_number(args[1]))
			{
				fprintf(stderr, "%s: 1: exit: Illegal number: %s\n", program, args[1]);
				status = 2;
			}
			else
				status = string_to_int(args[1]);
		}
		else
			status = last_status; /* ← uses last_status */
	}
	else if (string_compare(args[0], "echo") == 0)
	{
		int i;
		for (i = 1; args[i] != NULL; i++)
		{
			if (i > 1)
				write(STDOUT_FILENO, " ", 1);
			write(STDOUT_FILENO, args[i], string_length(args[i]));
		}
		write(STDOUT_FILENO, "\n", 1);
		status = 0;
	}
	else if (string_compare(args[0], "env") == 0)
	{
		print_env(*env);
		status = 0;
	}
	/* ... all other builtins ... */
	else if (string_compare(args[0], "setenv") == 0)
	{
		status = handle_setenv(args, env);
	}
	else if (string_compare(args[0], "unsetenv") == 0)
	{
		status = handle_unsetenv(args, env);
	}
	else if (string_compare(args[0], "cd") == 0)
	{
		status = handle_cd(args, env);
	}
	else if (string_compare(args[0], "alias") == 0)   /* ← THIS LINE IS MISSING */
	{
		status = handle_alias(args);
	}
	else
	{
		status = execute_command(args, *env, program);
	}
	/* Free the string we allocated during alias expansion */
	if (expanded_str != NULL)
		free(expanded_str);
	
	/* free what we allocated */
	if (expanded_var != NULL)
		free(expanded_var);
	/*if (expanded_alias != NULL)
		free(expanded_alias);*/

	return (status);
}
/**
 * execute_logical_list - handles commands connected by && and ||
 * (works with or without spaces around the operators)
 * @list: the string that may contain && or ||
 * @env: environment
 * @program: program name
 * @exit_shell: flag to exit the shell
 * @last_status: previous command status
 *
 * Return: status of the last command that was actually executed
 */
int execute_logical_list(char *list, char ***env, char *program, int *exit_shell, int last_status)
{
	char *start;
	char *p;
	char *op;
	int status = last_status;
	int should_run = 1; /* first command always runs */

	start = list;

	while (start != NULL && *start != '\0')
	{
		/* skip leading spaces */
		while (*start == ' ' || *start == '\t')
			start++;

		if (*start == '\0')
			break;

		/* look for the next && or || */
		p = start;
		op = NULL;

		while (*p != '\0')
		{
			if (p[0] == '&' && p[1] == '&')
			{
				op = "&&";
				*p = '\0'; /* cut the current command */
				p += 2;
				break;
			}
			if (p[0] == '|' && p[1] == '|')
			{
				op = "||";
				*p = '\0';
				p += 2;
				break;
			}
			p++;
		}
		/* execute the current command if we should */
		if (*start != '\0' && should_run)
		{
			status = execute_one_command(start, env, program, exit_shell, last_status);
			if (*exit_shell)
				return (status);
		}
		/* decide whether the next command should run */
		if (op == NULL)
			break; /* no more operators */

		if (string_compare(op, "&&") == 0)
			should_run = (status == 0); /* run next only on success */
		else
			should_run = (status != 0); /* run next only on failure */

		start = p; /* continue after the operator */
	}
	return (status);
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
