// Tilde Expansion implementation file

#include <stdlib.h>
#include "shell.h"

char * Tilde()
{
	/* simply returns the cstring value of $HOME env var for use
	 * by calling function */

	return getenv("HOME");
}
