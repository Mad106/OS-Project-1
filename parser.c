#include "shell.h"

extern char* strdup(const char*);
char *get_input(void);
tokenlist *get_tokens(char *input);
tokenlist *new_tokenlist(void);
tokenlist *clone_tokenlist(tokenlist *);
void add_token(tokenlist *tokens, char *item);
void free_tokens(tokenlist *tokens);

static bgjobslist bg;

int main()
{
    // get shell start time
    time(&shellStart);

    bg.size = 0;
    bg.jobs = NULL;
  

   // setup variable to keep track of longest process
   longestProc = 0;

	while(1)
	{
		// process start var
		time_t procStart;
		time(&procStart);
	
		// print the required prompt values
		Prompt();

		printf("> ");

		/* input contains the whole command
		 * tokens contains substrings from input split by spaces
		 */

		char *input = get_input();
		/* printf("whole input: %s\n", input); */

		tokenlist *tokens = get_tokens(input);
		/*printf("%d\n", tokens->size);
		for (int i = 0; i < tokens->size; i++) {		
			tokens->items[i] = (char*)EVar(tokens->items[i]);
			printf("token %d: (%s)\n", i, tokens->items[i]);
		}*/

		free(input);

		// pass tokenlist to Path() to locate and run command
		if(tokens->size > 0)
			Path(tokens, &bg, procStart);

		free_tokens(tokens);
	}
	for(int i = 0; i < bg.size; ++i) {
		free_tokens(bg.jobs[i]->tokens);
		free(bg.jobs[i]);
	}
	
	return 0;
}

tokenlist *new_tokenlist(void)
{
	tokenlist *tokens = (tokenlist *) malloc(sizeof(tokenlist));
	tokens->size = 0;
	tokens->items = (char **) malloc(sizeof(char *));
	tokens->items[0] = NULL; /* make NULL terminated */
	return tokens;
}

tokenlist *clone_tokenlist(tokenlist *tokens) {
	tokenlist *newtokens = (tokenlist *) malloc(sizeof(tokenlist));
	newtokens->size = 0;
	newtokens->items = (char **) malloc(sizeof(char *) * (tokens->size + 1));
	newtokens->items[tokens->size] = NULL; /* make NULL terminated */
	
	for(int i = 0; i < tokens->size; ++i) {
		newtokens->items[i] = strdup(tokens->items[i]);
	}
	newtokens->size = tokens->size;
	
	return newtokens;
}

void add_token(tokenlist *tokens, char *item)
{
	int i = tokens->size;
	int itemLen = strlen(item);

	tokens->items = (char **) realloc(tokens->items, (i + 2) * sizeof(char *));
	tokens->items[i] = (char *) malloc(itemLen + 1);
	memset(tokens->items[i], 0, sizeof(char) * itemLen);
	strcpy(tokens->items[i], item);
	tokens->items[i + 1] = NULL;
	++tokens->size;
}

char *get_input(void)
{
	char *buffer = NULL;
	int bufsize = 0;

	char line[5];
	while (fgets(line, 5, stdin) != NULL) {
		int addby = 0;
		char *newln = strchr(line, '\n');
		if (newln != NULL)
			addby = newln - line;
		else
			addby = 5 - 1;

		buffer = (char *) realloc(buffer, bufsize + addby);
		memcpy(&buffer[bufsize], line, addby);
		bufsize += addby;

		if (newln != NULL)
			break;
	}

	buffer = (char *) realloc(buffer, bufsize + 1);
	buffer[bufsize] = 0;

	return buffer;
}

tokenlist *get_tokens(char *input)
{
	int len = strlen(input);
	char *buf = (char *) malloc(len + 1);
	memset(buf, 0, len + 1);
	strcpy(buf, input);

	tokenlist *tokens = new_tokenlist();

	char *tok = strtok(buf, " ");
	while (tok != NULL) {
		add_token(tokens, tok);
		tok = strtok(NULL, " ");
	}

	free(buf);
	return tokens;
}

void free_tokens(tokenlist *tokens)
{
	for (int i = 0; i < tokens->size; i++)
		free(tokens->items[i]);
	free(tokens->items);
	free(tokens);
}
