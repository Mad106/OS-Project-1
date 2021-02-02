#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "shell.h"

extern void free_tokens(tokenlist *tokens);

void Jobs(bgjobslist* bg, int showAll) {

	if(bg == NULL) return;
	int activeJobs = 0;
	for(int i = 0; i < bg->size; ++i) {
		/* Get status of the child */
		int status = waitpid(bg->jobs[i]->pid, NULL, WNOHANG);

		/* If job is running (status == 0) then we will print all tokens, otherwise 
		we should drop '&' token, which is last in the list */
		int totalTokens = status == 0 ? bg->jobs[i]->tokens->size : bg->jobs[i]->tokens->size - 1;

		/* Print job information if we should print any type of job (completed/running) OR current status of job is "done" */
		if(showAll || status != 0) {
			printf("[%d] %s\t", bg->jobs[i]->jobId, (status == 0 ? "Running" : "Done"));
			for(int j = 0; j < totalTokens; ++j) {
				printf("%s", bg->jobs[i]->tokens->items[j]);
				printf(" ");
			}
			printf("\n");
		}

		/* Remove record for terminated childs */
		if(status != 0) {
			free_tokens(bg->jobs[i]->tokens);
			free(bg->jobs[i]);
			bg->jobs[i] = NULL;
		} else { /* Keep counting alive childs */
			++activeJobs;
		}
		
	}
	
	/* Pack array of jobs (shrink due NULL elements) */
	for(int i = 0; i < bg->size; ++i) {
		if(bg->jobs[i] == NULL) {
			/* Search for first non null element */
			for(int j = i + 1; j < bg->size; ++j) {
				if(bg->jobs[j] != NULL) {
					bg->jobs[i] = bg->jobs[j];
					bg->jobs[j] = NULL;
				}
			}
		}
	}
	bg->size = activeJobs;
}
