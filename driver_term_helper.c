#include "driver_internal.h"

#include <curses.h>
#include <term.h>

void driver_term_write(const char *string)
{
	if (string == NULL)
		return;
	putp(string);
}
