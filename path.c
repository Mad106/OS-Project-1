#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shell.h"

void Path(char ** tokens)
{
	// get copy of $PATH variable
	char * paths = (char*) malloc(strlen(getenv("PATH"))+1);
	strcpy(paths, getenv("PATH"));
	
	// create array of cstrings to hold each directory path
	char ** separatedPaths = (char **) malloc(sizeof(char*));

	// separate directories by path - begins outside loop to satisfy
	// strtok requirement that subsequent calls pass NULL
	char * tok = strtok(paths, ":");

	// add paths to separatedPaths and append filename to path
	for(int i = 0; tok != NULL; ++i)
	{
		// rellocate the size of separatedPaths to include the path
		separatedPaths = (char**) realloc(separatedPaths, (sizeof(tok)*sizeof(char*))+
				1+(sizeof(tokens[0])*sizeof(char*)));
		// allocate the amount of space necessary in separatedPaths[i]
		separatedPaths[i] = (char *) malloc(strlen(tok)+1+(strlen(tokens[0])));

		// concat the entire filepath into separatedPaths[i]
		strcat(separatedPaths[i], tok);
		strcat(separatedPaths[i], "/");		
		strcat(separatedPaths[i], tokens[0]);

		// call strtok to get next token
		tok = strtok(NULL, ":");
	}

	// get number of elements in separatedPaths
	int size = sizeof(separatedPaths);

	/* loop through separatedPaths paths and check for file.
	 * access() checks if the file exists at that location.
	 * if file exists, fork, run command, & free resources
	*/
	for(int i = 0; i < size; ++i)
	{
		if(access(separatedPaths[i], F_OK) == 0)
		{
			// create fork so shell doesn't shut down
			int pid = fork();
			if(pid == 0)
				execv(separatedPaths[i],tokens);
			else
				waitpid(pid, NULL, 0);

			// process complete, return to main
			return;			
		}
	}

	/* if previous check did not locate the command at any $PATH paths
	 * check the command provided by user to see if a direct path
	 * was given. If found, run same process detailed above
	*/
	if(tokens[0][0] == '.' || tokens[0][0] == '/')
	{
		if(access(tokens[0], F_OK) == 0)
		{
			// create fork so shell doesn't shut down
			int pid = fork();
			if(pid == 0)
				execv(tokens[0], tokens);
			else
				waitpid(pid, NULL, 0);

			// process complete, return to main
			return;
		}
	}		
		

	/* If this point is reached, the command could not be found.
	 * Print error message and free resources
	*/
	printf("shell: %s: command not found\n", tokens[0]);
}
