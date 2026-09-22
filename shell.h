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
char *build_path(char *dir, char *command);
void split_line(char *line, char **args);
void print_env(char **env);
char *get_path(char **env);
char *find_command(char *command, char **env);
int execute_command(char **args, char **env, char *program);
int process_line(char *line, char ***env, char *program, int *exit_shell, int last_status);
char *string_duplicate(char *str);
char *find_character(char *str, char character);
int string_compare(char *s1, char *s2);
char *read_line(void);
int string_to_int(char *str);
int is_number(char *str);
int handle_setenv(char **args, char ***env);
int string_length(char *str);
int string_starts_with(char *str, char *prefix);
char **copy_environment(char **env);
void free_environment(char **env);
int add_environment(char *variable, char ***env);
int handle_unsetenv(char **args, char ***env);
#endif
