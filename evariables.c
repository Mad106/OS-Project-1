#include <stdlib.h>
#include "shell.h"

const char* EVar(const char *name)
{
	//only return something different if starts with '$'
	if(name[0] == '$'){
		char* temp = (char*) malloc(sizeof(name) - 1);
		for(size_t i = 1; i < sizeof(name); i++)
			temp[i-1] = name[i];
		char* token = temp;
		free(temp);
		return getenv(token);
	}else
		return name;
}


/* One last thing: you may not be working with a copy of the environment variable.
 * I had the same syntax in path.c and ended up erasing the contents of $PATH.
 * I'm not sure if you've had any of these issues on your end, but if you run into
 * them, I've got a couple ideas if you want to call before monday and brainstorm
 * for a bit. Let me know!
*/
