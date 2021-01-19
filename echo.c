// Echo command implementation file

#include <stdio.h>
#include "shell.h"

void Echo(int size, char ** args)
{
	/* args is the tokens var passed in from parser.c.
	 * The for loop goes through each of the elements 
	 * of args, which are the individual tokens entered
	 * by the user, and prints them followed by a space

	/* skip args[0] because that's the echo command
	 * print args[1]-arg[size-1] */
	for(int i = 1; i < size; ++i) 	
		printf("%s ",args[i]);

	// print new line for formatting
	printf("\n");
}
