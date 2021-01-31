#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "shell.h"

void CD(tokenlist* tokens)
{
	if(tokens->size > 2){
		//error more than one argument
		printf("cd: too many arguments");
	}else if(chdir(tokens->items[1]) != 0){
		//if directory not found
		printf("cd: directory not found");
	}else{
		char* cwd = getcwd(NULL, 0);
		if(setenv("PWD", cwd, 1) != 0){
			//target not a directory
			printf("cd: target is not directory");
		}
		free(cwd); 
	}
	
	return;
}