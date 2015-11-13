#include "driver_internal.h"

#include <curses.h>
#include <term.h>

void driver_term_init(void)
{
	int err = 0;
	if (setupterm(NULL, 1, &err) == ERR) {
		if (err == -1) {
			printf("The terminfo database could not be found.\n");
		}
		if (err == 0) {
			printf("You are using a generic or unknown setting for TERM,\n");
		}
		printf("You can try manually specifying an output mode.\n");
		exit(1);
	}
}

void driver_term_write(const char *string)
{
	if (string == NULL)
		return;
	putp(string);
}
