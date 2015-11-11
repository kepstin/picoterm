#include "charset.h"
#include "driver.h"

#include <curses.h>
#include <term.h>

#include <locale.h>
#include <langinfo.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define GETTEXT_PACKAGE "picoterm"
#include <glib/gi18n.h>

/* TODO: man tparm */

int main(void) {
	setlocale(LC_ALL, "");
	textdomain(GETTEXT_PACKAGE);

	struct driver *driver = driver_get_default();

	driver_test(driver);

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


	if (lines < 0) {
		printf("Could not get number of terminal lines\n");
	}
	if (columns < 0) {
		printf("Could not get number of terminal columns\n");
	}
	if (max_colors < 0) {
		printf("Max colors not available, monochrome terminal?\n");
	}
	if (max_colors <= 8) {
		printf("The terminal description currently used only supports %d colors.\n", max_colors);
		printf("If your terminal supports more colors than that, please set TERM to a closer\n");
		printf("match, or use override options.\n");
	}
	printf("Terminal appears to be %dx%d\n", columns, lines);
	printf("Terminfo knows how to use %d colors\n", max_colors);

	driver_free(driver);
	driver = NULL;

	printf("Testing semigraphics abilities:\n");
	struct charset *charset = charset_get_default(
		CHARSET_INVERSE | CHARSET_RES_HALF | CHARSET_RES_QUARTER |
		CHARSET_SHADE | CHARSET_UTF8_EXTENDED);
	printf("Character set: %s (%s)\n",
			charset_get_name(charset),
			charset_get_description(charset));
	size_t chars = 0;
	const struct glyph **glyph = charset_get_glyphs(charset, &chars);
	putp(charset_get_enter_string(charset));
	for (size_t i = 0; i < chars; i++)
		fputs(glyph[i]->code, stdout);
	putp(charset_get_exit_string(charset));
	putc('\n', stdout);
	charset_free(charset);
	return 0;
}
