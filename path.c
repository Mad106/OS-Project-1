#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include "shell.h"


extern char* strdup(const char*);
extern int setenv(const char *name, const char *value, int overwrite);

extern tokenlist *clone_tokenlist(tokenlist *tokens) ;
extern void free_tokens(tokenlist *tokens);


void Path(tokenlist *tokens, bgjobslist* bg, time_t procStart)
{
	static int jobId = 1;

	/* Build in functions */
	if(strcmp(tokens->items[0], "cd") == 0) {
		CD(tokens);

		/* Check background jobs */
		Jobs(bg, 0, procStart); /* Zero for "SHOW ONLY COMPLETED", One for "SHOW ALL" */
		
		return;
	}
	
	if(strcmp(tokens->items[0], "jobs") == 0) {
		/* Check background jobs */
		Jobs(bg, 1, procStart); /* Zero for "SHOW ONLY COMPLETED", One for "SHOW ALL" */
		return;
	}
	
	if(strcmp(tokens->items[0], "exit") == 0 || strcmp(tokens->items[0], "quit") == 0) {
		/* Cleanup job list */
		for(int i = 0; i < bg->size; ++i) { 
			free_tokens(bg->jobs[i]->tokens); 
			free(bg->jobs[i]);
		}
		free(bg->jobs);
		/* Cleanup token list */
		free_tokens(tokens);
		
		/* Wait for childs and exit */
		waitpid(-1, NULL, 0);

		// get exit time
		time_t shellEnd;
		time(&shellEnd);
		time_t shellRun = shellEnd - shellStart;
		long longestProc = 0;
		printf("Shell ran for %li seconds and took %li seconds to execute one command.\n",
			shellRun, longestProc);
		exit(0);
	}

	// get copy of $PATH variable
	char* varPath = getenv("PATH");
	char * paths = (char*)strdup(varPath);
	
	// create array of cstrings to hold each directory path
	char ** separatedPaths = NULL;

	// separate directories by path - begins outside loop to satisfy
	// strtok requirement that subsequent calls pass NULL
	char * tok = strtok(paths, ":");
	
	int totalParts = 0;
	// add paths to separatedPaths and append filename to path
	for(int i = 0; tok != NULL; ++i)
	{
		// rellocate the size of separatedPaths to include the path
		separatedPaths = (char**) realloc(separatedPaths, sizeof(char*) * (i + 1) );
		// allocate the amount of space necessary in separatedPaths[i]
		int len = strlen(tok) + 1 + (strlen(tokens->items[0]));
		separatedPaths[i] = (char *) malloc(sizeof(char) * (len + 1));
		memset(separatedPaths[i], 0, sizeof(char) * (len + 1));

		// concat the entire filepath into separatedPaths[i]
		strcpy(separatedPaths[i], tok);
		strcat(separatedPaths[i], "/");		
		strcat(separatedPaths[i], tokens->items[0]);

		// call strtok to get next token
		tok = strtok(NULL, ":");
		++totalParts;
	}
	free(paths);

	// get number of elements in separatedPaths
	int size = totalParts;

	/* loop through separatedPaths paths and check for file.
	 * access() checks if the file exists at that location.
	 * if file exists, fork, run command, & free resources
	*/
	for(int i = 0; i < size; ++i)
	{
		if(access(separatedPaths[i], F_OK) == 0)
		{
			int isBG = 0;
			/* Background job */
			if(tokens->items[tokens->size - 1] != NULL && strcmp(tokens->items[tokens->
					size - 1], "&") == 0) {
				/* Append background job into array of jobs */
				bg->jobs = (bgjob_t**)realloc(bg->jobs, sizeof(bgjob_t*) * 
					(bg->size + 1));
				bg->jobs[bg->size] = (bgjob_t*)malloc(sizeof(bgjob_t));
				memset(bg->jobs[bg->size], 0, sizeof(bgjob_t));
				bg->jobs[bg->size]->jobId = jobId;
				bg->jobs[bg->size]->tokens = clone_tokenlist(tokens);
				
				/* Cleanup */				
				free(tokens->items[tokens->size - 1]);
				tokens->items[tokens->size - 1] = NULL;
				tokens->size = tokens->size - 1;
				isBG = 1;
			}
			
			/* Check for redirect */
			char* redirInput = NULL;
			char* redirOutput = NULL;
			bool piping = false;
			int inFd, outFd;
			
			/* Move backwards and check for "<" and ">" symbols */
			for(int j = tokens->size - 1; j >= 0; --j) 
			{
				/* Input redirect found */	
				if(redirInput == NULL && strcmp(tokens->items[j], "<") == 0 && 
					j + 1 < tokens->size) 
				{					
					redirInput = tokens->items[j + 1];
					free(tokens->items[j]);
					tokens->items[j] = tokens->items[j + 1] = NULL;
				} 
				/* Output redirect found */
				else if(redirOutput == NULL && strcmp(tokens->items[j], ">") == 0 
					&& j + 1 < tokens->size) 
				{
					redirOutput = tokens->items[j + 1];
					free(tokens->items[j]);
					tokens->items[j] = tokens->items[j + 1] = NULL;	
				}
				/* Piping found */
				else if(redirOutput == NULL && redirInput == NULL && strcmp(tokens->items[j], "|") == 0 && j + 1 < tokens->size)
				{
					piping = true;
					redirOutput = tokens->items[j - 1];
					redirInput = tokens->items[j + 1];
					free(tokens->items[j]);
					tokens->items[j] = tokens->items[j + 1] = NULL;
				}
			}
					
			/* Open input file */
			if(redirInput) 
			{
						inFd = open(redirInput, O_RDONLY);
				printf("[REDIRECT INPUT]=[%s]\n", redirInput);
        		}
            		if(redirOutput) 
			{
            			outFd = open(redirOutput, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
				printf("[REDIRECT OUTPUT]=[%s]\n", redirOutput);
            		}
		
			/* only used for piping */
			if(piping)
			{
				int p_fds[2];
				pipe(p_fds);
				
				int pid1 = fork();
				if(pid1 == 0){
					close(p_fds[0]);	//close unused end of pipe
					//redirect output & command 1
					close(STDOUT_FILENO);
					dup(outFd);
					close(outFd);
					system(redirOutput);
					exit(1);
				}
				
				int pid2 = fork();
				if(pid2 == 0){
					close(p_fds[1]);	//close unused end of pipe
					//redirect input & command 2
					close(STDIN_FILENO);
					dup(inFd);
					close(inFd);
					system(redirInput);
					exit(1);
				}
				
				close(p_fds[0]);
				close(p_fds[1]);
				
				waitpid(pid1,NULL,0);
				waitpid(pid2,NULL,0);
				
				execv(separatedPaths[i], tokens->items);
				exit(-1);
			}
			else
			{
				// create fork so shell doesn't shut down
				int pid = fork();
				if(pid == 0) 
				{
					if(redirInput) 
					{
						close(STDIN_FILENO);
						dup(inFd);
						close(inFd);
						//execute command
						system(redirInput);
					}

					if(redirOutput) 
					{
						close(STDOUT_FILENO);
						dup(outFd);
						close(outFd);
						//execute command
						system(redirOutput);
					}
					
					execv(separatedPaths[i], tokens->items);
					exit(-1);
				} 
				else 
				{
					/* Parent cleanup */
					if(redirInput) 
					{
						close(inFd);
						free(redirInput);
					}
					if(redirOutput) 
					{
						close(outFd);
						free(redirOutput);
					}
					
					/* Do not wait for background job */
					if(isBG) 
					{
						/* Save pid of the child process */
						bg->jobs[bg->size]->pid = pid;
						/* Show message that bg job is running */
						printf("[%d] %d\n", jobId, pid);
						++bg->size;
						++jobId;
						//waitpid(pid, NULL, WNOHANG);
					} 
					else 
					{
						/* Wait for child */
						waitpid(pid, NULL, 0);

						// calculate program run time
						time_t procEnd;
						time(&procEnd);
						long procTime = 0;
						procTime = procEnd - procStart;
						if(procTime > longestProc) longestProc = procTime;
					}
				}
			}

			/* Clean up */
			for(int j = 0; j < size; ++j) { free(separatedPaths[j]); }
			free(separatedPaths);

			/* Check background jobs */
			/* Zero for "SHOW ONLY COMPLETED", One for "SHOW ALL" */
			Jobs(bg, 0, procStart); 

			// process complete, return to main
			return;			
		}
	}
	
	/* Clean up */
	for(int i = 0; i < size; ++i) { free(separatedPaths[i]); }
	free(separatedPaths);
	
	/* if previous check did not locate the command at any $PATH paths
	 * check the command provided by user to see if a direct path
	 * was given. If found, run same process detailed above
	*/
	if(tokens->items[0][0] == '.' || tokens->items[0][0] == '/')
	{
		if(access(tokens->items[0], F_OK) == 0)
		{
			int isBG = 0;
			/* Background job */
			if(tokens->items[tokens->size - 1] != NULL && strcmp(tokens->items[tokens->
					size - 1], "&") == 0) {
				/* Append background job into array of jobs */
				bg->jobs = (bgjob_t**)realloc(bg->jobs, sizeof(bgjob_t*) * 
					(bg->size + 1));
				bg->jobs[bg->size] = (bgjob_t*)malloc(sizeof(bgjob_t));
				memset(bg->jobs[bg->size], 0, sizeof(bgjob_t));
				bg->jobs[bg->size]->jobId = jobId;
				bg->jobs[bg->size]->tokens = clone_tokenlist(tokens);
				
				/* Cleanup */				
				free(tokens->items[tokens->size - 1]);
				tokens->items[tokens->size - 1] = NULL;
				tokens->size = tokens->size - 1;
				isBG = 1;
				
			}
		
			// create fork so shell doesn't shut down
			int pid = fork();
			if(pid == 0) {
				execv(tokens->items[0], tokens->items);
			} else {
				/* Do not wait for background job */	
				if(isBG) {
					/* Save pid of the child process */
					bg->jobs[bg->size]->pid = pid;
					/* Show message that bg job is running */
					printf("[%d] %d\n", jobId, pid);
					++bg->size;
					++jobId;
					
					//waitpid(pid, NULL, WNOHANG);
				} else {
					/* Wait for child */
					waitpid(pid, NULL, 0);

					// calculate program run time
					time_t procEnd;
					time(&procEnd);
					long procTime = 0;
					procTime = procEnd - procStart;
					if(procTime > longestProc) longestProc = procTime;
				}
			}

			/* Check background jobs */
			/* Zero for "SHOW ONLY COMPLETED", One for "SHOW ALL" */
			Jobs(bg, 0, procStart);
		
			// process complete, return to main
			return;
		}
	}
	
	
	/* If this point is reached, the command could not be found.
	 * Print error message and free resources
	*/
	printf("shell: %s: command not found\n", tokens->items[0]);
	
	/* Check background jobs */
	Jobs(bg, 0, procStart); /* Zero for "SHOW ONLY COMPLETED", One for "SHOW ALL" */
}
