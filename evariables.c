#include <stdlib.h>
#include "shell.h"

char* EVar(const char *name)
{
	//only return something different if starts with '$'
	if(name[0] == '$'){
		char* token = "";
		for(int i = 1; i < sizeof(name); i++)
			token[i-1] = name[i];
		return getenv(token);
	}else
		return name;
}

//do NOT want to free returned values
//Modifying the value modifies the underlying environment variable
//All modifications must be done on copies