#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
/**
 * remove_comments - Removes comments from a line of input.
 * @line: The input line.
 *
 * Return: The input line with comments removed.
 */
char *remove_comments(char *line)
{
    char *comment_start = strchr(line, '#');
    if (comment_start != NULL)
    {
        *comment_start = '\0';
    }
    return line;
}

/**
 * check_path - checks if the command is in the PATH
 * @command: command to check
 *
 * Return: command if it's in the PATH, NULL otherwise
 */
char *check_path(char *command)
{
    char *path = getenv("PATH"), *path2, *tok, *new_cmd;
    int i = 0, c = 0;

    while (command[i] != '\0')
    {
        if (command[i] == '/')
        {
            return (command);
        }
        i++;
    }
    path2 = strdup(path);
    tok = strtok(path2, ":");

    while (tok != NULL)
    {
        new_cmd = calloc(strlen(tok) + 2 + strlen(command), sizeof(char));
        strcpy(new_cmd, tok);
        new_cmd[strlen(tok)] = '/';
        strcat(new_cmd, command);
        new_cmd[strlen(new_cmd)] = '\0';
        c = access(new_cmd, F_OK);
        if (c == 0)
        {
            char *new_cmd_copy = strdup(new_cmd);

            free(new_cmd);
            free(path2);
            return (new_cmd_copy);
        }
        free(new_cmd);
        tok = strtok(NULL, ":");
    }
    free(path2);

    return (NULL);
}

/**
 * getlineterminal - reads a line from the terminal
 *
 * Return: buffer containing the line read
 */
char *getlineterminal(void)
{
    char *buffer;
    size_t bufsize = 40;
    ssize_t character;

    buffer = (char *)malloc(bufsize * sizeof(char));
    if (buffer == NULL)
    {
        perror("Unable to allocate buffer");
        exit(1);
    }
    character = getline(&buffer, &bufsize, stdin);
    if (character == -1)
    {
        if (feof(stdin))
        {
            free(buffer);
            exit(0);
        }
        else
        {
            perror("getline failed.");
            free(buffer);
            buffer = NULL;
            return (NULL);
        }
    }
    return (buffer);
}

/**
 * commandss - splits the input line into separate commands
 * @ch: input line
 *
 * Return: array of commands
 */
char **commandss(char *ch)
{
    char **commands;
    int i = 0;
    char *ch_copy = strdup(ch);

    commands = malloc(sizeof(char *));
    if (commands == NULL)
    {
        perror("Unable to allocate buffer");
        exit(1);
    }
    commands[0] = strdup(strtok(ch_copy, " \t\n"));
    while (commands[i] != NULL)
    {
        i++;
        commands = realloc(commands, (i + 1) * sizeof(char *));
        if (commands == NULL)
        {
            perror("Unable to allocate buffer");
            exit(1);
        }
        commands[i] = strtok(NULL, " \t\n");
        if (commands[i] != NULL)
        {
            commands[i] = strdup(commands[i]);
        }
    }
    free(ch_copy);
    
	return (commands);
}


/**
 * execute_commands - executes the parsed commands
 * @commands: parsed commands
 */
void execute_commands(char **commands)
{
    extern char **environ;
    char **env = environ;
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        exit(1);
    }

    if (pid == 0)
    {
        dup2(STDOUT_FILENO, STDERR_FILENO);

        if (commands[0] == NULL)
        {
            fprintf(stderr, "./hsh: command not found\n");
            exit(EXIT_FAILURE);
        }

        if (strcmp(commands[0], "env") == 0)
        {
            while (*env)
            {
                printf("%s\n", *env);
                env++;
            }
            exit(EXIT_SUCCESS);
        }

        commands[0] = check_path(commands[0]);

        if (commands[0] == NULL)
        {
            fprintf(stderr, "./hsh: %s: command not found\n", commands[0]);
            exit(EXIT_FAILURE);
        }

        execve(commands[0], commands, NULL);

        fprintf(stderr, "./hsh: %s: command not found\n", commands[0]);
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        wait(NULL);
    }
}

/**
 * change_directory - changes the current working directory
 * @commands: parsed commands
 */
void change_directory(char **commands)
{
	char *home = getenv("HOME");
	char cwd[1024];

	if (commands[1] == NULL || strcmp(commands[1], "~") == 0)
	{
		if (home == NULL)
		{
			fprintf(stderr, "cd: HOME not set\n");
			return;
		}
		chdir(home);
	}
	else if (strcmp(commands[1], "-") == 0)
	{
		char *oldpwd = getenv("OLDPWD");

		if (oldpwd == NULL)
		{
			fprintf(stderr, "cd: OLDPWD not set\n");
			return;
		}
		printf("%s\n", oldpwd);
		chdir(oldpwd);
	}
	else
	{
	    if(chdir(commands[1]) != 0)
	    {
		    fprintf(stderr, "./hsh: 1: cd: can't cd to %s\n", commands[1]);
		    return;
	    }
	}

	if(getcwd(cwd, sizeof(cwd)) != NULL)
	{
		setenv("PWD", cwd, 1);
	}
}
