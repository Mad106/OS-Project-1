#include <stdlib.h>
#include "shell.h"

const char* EVar(const char *name)
{
	//only return something different if starts with '$'
	if(name[0] == '$'){
		char* temp = (char*) malloc(sizeof(name) - 1);
		for(size_t i = 1; i < sizeof(name); i++)
			temp[i-1] = name[i];
		char* token = (char*) malloc(strlen(getenv(temp)));
		strcpy(token, getenv(temp));
		free(temp);

		return token;
	}else
		return name;
}
