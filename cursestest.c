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

void main(void) {
	setlocale(LC_ALL, "");
	textdomain(GETTEXT_PACKAGE);

	const struct driver *driver = driver_get_default();

	driver_test(driver);

	char *codeset = nl_langinfo(CODESET);
	bool have_utf8 = true;
	if (strcmp(codeset, "UTF-8") != 0) {
		have_utf8 = false;
	}

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

	printf("Testing semigraphics abilities:\n");
	if (have_utf8) {
		printf("UTF-8\n");
		size_t chars = 0;
		const struct glyph **charset = charset_utf8(&chars,
			CHARSET_INVERSE|CHARSET_UTF8_EXTENDED);
		for (size_t i = 0; i < chars; i++)
			printf("%s", charset[i]->code);
		putp(exit_attribute_mode);
		printf("\n");
		free(charset);
	}
	if (enter_alt_charset_mode) {
		printf("Alternate character set:\n");
		size_t chars = 0;
		const struct glyph **charset = charset_acs(&chars,
			CHARSET_INVERSE);
		putp(enter_alt_charset_mode);
		for (size_t i = 0; i < chars; i++)
			printf("%s", charset[i]->code);
		putp(exit_alt_charset_mode);
		printf("\n");
		free(charset);
	}
}
