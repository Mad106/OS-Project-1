#include <stdlib.h>
#include "shell.h"

const char* EVar(const char *name)
{
	//only return something different if starts with '$'
	if(name[0] == '$'){
		char* var = getenv(&name[1]);
		int len = strlen(var);
		char* token = (char*) malloc(sizeof(char) * len);
		memset(token, 0, sizeof(char) * len);
		strcpy(token, var);
		free((char*)name);

		return token;
	} else if(name[0] == '~') {
		char * homeVar = getenv("HOME");
		/* Find total length of the result string */
		int totalLen = strlen(&name[1]) + strlen(homeVar) + 1;
		char* token = (char*) malloc(sizeof(char) * totalLen);
		memset(token, 0, sizeof(char) * totalLen);
		strcpy(token, homeVar);
		strcat(token, &name[1]);
		free((char*)name);

		return token;
	} else {
		
		return name;
	}
}
