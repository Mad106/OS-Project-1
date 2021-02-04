#ifndef _BSD_SOURCE
#define _BSD_SOURCE

#include "shell.h"

void CD(tokenlist* tokens)
{	/* Two step process */
	/* 1. Change dir */
	if(tokens->size > 2){
		//error more than one argument
		printf("cd: too many arguments");
	}else if(chdir(tokens->items[1]) != 0){
		//if directory not found
		printf("cd: directory not found");
	}else{
		/* 2. Set new PWD */
		char* cwd = getcwd(NULL, 0);
		if(setenv("PWD", cwd, 1) != 0){
			//target not a directory
			printf("cd: target is not directory");
		}
		free(cwd); 
	}
	
	return;
}

#endif
