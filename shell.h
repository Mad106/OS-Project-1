// shell header file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _SHELL_H
#define _SHELL_H

/* This header file is used to link together the individual
 * C files, allowing the functions to be called between each
 * one without errors */

void Echo(int, char**);
void Prompt();
void Parse();
char * Tilde();
char* EVar(const char *name);
void Path(char**);

#endif
