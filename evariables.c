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




/* instead of int i = 1, we should use size_t to prevent a comparison warning since
 * sizeof() returns size_t. return name; is also throwing a warning since name is 
 * const. We could copy the contents to a non const local variable and return that
 * instead? Also, when testing this function I keep getting a segfault. Based on
 * some playing around, I think it's caused by char* token = "". I tried allocating
 * memory using char* token = (char*) malloc(sizeof(name)), and that worked in this
 * function but caused a memory leak and crash at the end of the main routine.
 * One last thing: you may not be working with a copy of the environment variable.
 * I had the same syntax in path.c and ended up erasing the contents of $PATH.
 * I'm not sure if you've had any of these issues on your end, but if you run into
 * them, I've got a couple ideas if you want to call before monday and brainstorm
 * for a bit. Let me know!
*/
