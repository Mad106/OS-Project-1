// shell header file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

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

void Echo(int, char**);
void Prompt();
void Parse();
char * Tilde();
const char* EVar(const char *name);
void Path(tokenlist *tokens, bgjobslist* jobs);
/* Background jobs function */
void Jobs(bgjobslist* jobs, int showAll);

#endif
