#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

int main()
{
	extern char **environ;
	char *line = NULL;
	char **command = NULL;
	char **env = environ;
	int i = 0, j;

	while (1)
    {
	    line = getlineterminal();
	    
	    j = 0;
	    while (line[j] != '\0')
	    {
		    if(!isspace(line[j])) 
		    {
			    break;
		    }
		    j++;
	    }
	    if (line[j] == '\0') 
	    {
		    free(line); line=NULL;
		    continue;
	    }

	    command = commandss(line);

	    if (strcmp(*command, "exit") == 0)
	    {
		    free(line); line=NULL;

		    while (command[i] != NULL)
		    {
			    free(command[i]);
			    command[i] = NULL; 
			    i++;
		    }
		    free(command); command=NULL;
		    break;
	    }

	    if (strcmp(*command, "env") == 0)
	    {
		    env = environ;
		    while (*env)
		    {
			    printf("%s\n",*env);
			    env++;
		    }
	    }
	    else if (strcmp(*command, "cd") == 0)
	    {
		    change_directory(command);
	    }
	    else
	    {
		    execute_commands(command);
	    }

	    i = 0;
	    while (command[i] != NULL)
	    {
		    free(command[i]);
		    command[i] = NULL; 
		    i++;
	    }		
        free(line); line=NULL;
        free(command); command=NULL;
    }	
	return (0);
}
