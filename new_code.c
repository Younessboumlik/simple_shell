#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

char *check_path(char *command)
{
	char *path=getenv("PATH"),*path2,*tok,*new_cmd;
	int i = 0,c;

	while(command[i] != '\0')
	{
		if (command[i] == '/')
		{
			return command;
		}
		i++;
	}
	path2 = strdup(path);
	tok = strtok(path2,":");

	while(tok != NULL)
	{
		new_cmd = malloc((strlen(tok)+2+strlen(command))*sizeof(char));
		strcpy(new_cmd,tok);
		new_cmd[strlen(tok)] = '/';
		strcat(new_cmd,command);
		new_cmd[strlen(new_cmd)] = '\0';		
		c = access(new_cmd,F_OK);
		if (c == 0)
		{
			return new_cmd;
			free(new_cmd);
		}
		free(new_cmd);
		tok = strtok(NULL,":");
	}
	free(path2);
	free(tok);
	return (NULL);
}


char *getlineterminal()
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
	character = getline(&buffer,&bufsize,stdin);
	if (character == -1)
	{
		if (feof(stdin))
		{
			exit(0);
		}
		else
			perror("getline failed.");
		free(buffer);
		return (NULL);
	}
	return (buffer);
}
char **commandss(char *ch)
{
	char **commands;
	int i = 0;
	char *ch_copy = strdup(ch);

	commands = malloc(sizeof(char *));
	commands[0] = strdup(strtok(ch_copy," \t\n"));
	while (commands[i] != NULL)
	{
		i++;
		commands = realloc(commands, (i + 1)*sizeof(char *));
		commands[i] = strtok(NULL," \t\n");
		if (commands[i] != NULL) {
			commands[i] = strdup(commands[i]);
		}
	}
	free(ch_copy);
	return (commands);
}

void execute_commands(char **commands)
{
	pid_t pid = fork();
	commands[0] = check_path(commands[0]);

	if (pid == 0)
	{
		execve(commands[0], commands, NULL);
		perror("execve");
		exit(EXIT_FAILURE);
	}
	else if (pid > 0)
	{
		wait(NULL);
	}
	else
	{
		perror("fork");
		exit(1);
	}
}

int main()
{
	extern char **environ;
	char *line = NULL;
	char **command = NULL;
	char **env = environ;
	int i;

	while (1)
	{
		line = getlineterminal();
		command = commandss(line);

		if (strcmp(*command, " ") == 0)
		{
			
		}
		if (strcmp(*command, "env") == 0)
		{
			while (*env)
			{
				printf("%s\n",*env);
				env++;
			}
			env = environ;
		}
		else
		{
			execute_commands(command);
		}
		i = 0;
		while (command[i] != NULL)
		{
			free(command[i]);
			i++;
		}		
		free(line);
		free(command);
	}	
	return (0);
}										    
