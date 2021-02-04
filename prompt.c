/* File that prints shell prompt for input.
 * NOTES: Do we need to add some type of error checking for null variables?
 */

#include "shell.h"

void Prompt()
{
	/* declare variables to copy env variables. Copies protect original data */
	char * userVar = getenv("USER");
	char * hostVar = getenv("HOSTNAME");
	char * pwdVar = getenv("PWD");

	/* print prompt */
	printf("%s@%s : %s", userVar, hostVar, pwdVar);
}
