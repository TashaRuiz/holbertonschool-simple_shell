#ifndef SHELL_H
#define SHELL_H
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
/*#define MAX_ARGS 10
extern char **environ;*/

/**
 * build_path - builds and checks a command path
 * @dir: directory from PATH
 * @command: command to find
 *
 * Return: full path if found, otherwise NULL
 */
char *build_path(char *dir, char *command);

/**
 * split_line - splits input into arguments
 * @line: input line
 * @args: array where arguments are stored
 */
void split_line(char *line, char **args);

/**
 * print_env - prints the environment
 * @env: environment variables
 */
void print_env(char **env);

/**
 * get_path - gets PATH from environment
 * @env: environment variables
 *
 * Return: PATH value or NULL
 */
char *get_path(char **env);

/**
 * find_command - finds a command in PATH
 * @command: command to find
 * @env: environment variables
 *
 * Return: full path to command or NULL
 */
char *find_command(char *command, char **env);

/**
 * execute_command - executes a command
 * @args: command arguments
 * @env: environment variables
 * @program: program name
 *
 * Return: exit status of command
 */
int execute_command(char **args, char **env, char *program);

/**
 * process_line - processes one command line
 * @line: command line
 * @env: environment variables
 * @program: program name
 *
 * Return: 1 to exit shell, 0 otherwise
 */
int process_line(char *line, char **env, char *program, int *exit_shell);
char *string_duplicate(char *str);
char *find_character(char *str, char character);
int string_compare(char *s1, char *s2);
char *read_line(void);
int string_to_int(char *str);
#endif
