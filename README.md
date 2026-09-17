MY SIMPLE SHELL
DESCRIPTION

Simple Shell is a UNIX command-line interpreter written in C. It is a simplified version of the standard shell found on UNIX and Linux systems.

The purpose of this project is to understand how a shell works internally and to practice important concepts in C programming, including:

Process creation and management
System calls
Environment variables
The PATH
Command execution
Memory management
String manipulation
File descriptors
Error handling

The shell can operate in both interactive and non-interactive modes.
---------------------------------------------------------------------------------------------------------------------------------------------------

INSTALLATION

Clone the repository:

git clone <your-repository-url>

Move into the project directory:

cd simple_shell

Compile the shell using:

gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh
---------------------------------------------------------------------------------------------------------------------------------------------------

USAGE
INTERACTIVE MODE

Run the shell:

./hsh

You will see a prompt where you can enter commands:

$ ./hsh
($) ls
($) pwd
($) echo Hello
($) exit

The shell reads each command, finds the corresponding executable, creates a process, and executes the command.
-----------------------------------------------------------------------------------------------------------------------------------------------------

BUILT-IN COMMANDS

The shell supports built-in commands that are handled directly by the shell.

exit

Exits the shell.

($) exit
env

Displays the current environment variables.

($) env
-----------------------------------------------------------------------------------------------------------------------------------------------------

COMMAND EXECUTION

The shell searches for commands using the PATH environment variable.

For example:

($) ls

The shell searches the directories listed in PATH until it finds the executable.

An absolute path can also be used:

($) /bin/ls
-----------------------------------------------------------------------------------------------------------------------------------------------------

ERROR HANDLING

The shell displays an appropriate error message when a command cannot be found or executed.

Example:

($) nonexistent
./hsh: 1: nonexistent: not found

The shell also handles memory allocation failures and other system-call errors where appropriate.
-----------------------------------------------------------------------------------------------------------------------------------------------------

PROJECT STRUCTURE

A typical project structure is:

simple_shell/
├── AUTHORS
├── README.md
├── man_1_simple_shell
├── hsh.c
├── shell.h
├── builtins.c
├── execute.c
├── helpers.c
└── ...

The exact files may vary depending on the implementation.
-----------------------------------------------------------------------------------------------------------------------------------------------------

MAIN COMPONENTS

Component	Purpose
hsh.c	Main shell loop
shell.h	Function prototypes and structures
builtins.c	Handles built-in commands
execute.c	Handles command execution
helpers.c	Utility/helper functions
man_1_simple_shell	Manual page
README.md	Project documentation
-----------------------------------------------------------------------------------------------------------------------------------------------------

COMPILATION

The project should be compiled with:

gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh

The compiler flags help identify errors and enforce the project's coding requirements.
-----------------------------------------------------------------------------------------------------------------------------------------------------

EXAMPLES
RUNNING A COMMAND

($) ls
README.md  hsh  shell.h
Printing the current directory
($) pwd
/home/user/simple_shell
Printing text
($) echo Hello World
Hello World
Displaying environment variables
($) env
HOME=/home/user
PATH=/usr/local/bin:/usr/bin:/bin
...
Exiting
($) exit
-----------------------------------------------------------------------------------------------------------------------------------------------------

OBJECTIVES

By completing this project, we learned how to:

Create and manage processes with fork()
Execute programs with execve()
Wait for child processes
Work with environment variables
Search for executables using PATH
Parse user input
Handle command-line arguments
Manage dynamically allocated memory
Handle errors in C
Work with UNIX system calls
Build a functional command interpreter
-----------------------------------------------------------------------------------------------------------------------------------------------------

AUTHOR

Tasha Arroyo Ruiz

