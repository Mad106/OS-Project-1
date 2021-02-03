// shell header file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>

#ifndef _SHELL_H
#define _SHELL_H

/* This header file is used to link together the individual
 * C files, allowing the functions to be called between each
 * one without errors */

typedef struct {
	int size;
	char **items;
} tokenlist;

typedef struct {
    int jobId;
    pid_t pid;
    tokenlist* tokens;
} bgjob_t;

typedef struct {
    int size;
    bgjob_t** jobs;
} bgjobslist;

// shell start time and longes process measurement
time_t shellStart;
long longestProc;

void Echo(int, char**);
void Prompt();
void Parse();
char * Tilde();
const char* EVar(const char *name);
void Path(tokenlist *tokens, bgjobslist* jobs, time_t procStart);
/* Background jobs function */
void Jobs(bgjobslist* jobs, int showAll, time_t procStart);
void CD(tokenlist* tokens);
void Piping(char* token1, char* token2);
void Exit();

#endif
