#include "driver_internal.h"
#include "charset.h"

#include <locale.h>
#include <glib/gi18n.h>
#include <stdlib.h>

const struct driver *driver_get_default(void) {
	return driver_get_rgb(TRUE);
}

const char *driver_get_name(const struct driver *driver) {
	return driver->name;
}

const char *driver_get_description(const struct driver *driver) {
	return gettext(driver->description);
}

guint32 driver_get_fg_colors(const struct driver *driver) {
	return driver->fg_colors;
}

guint32 driver_get_bg_colors(const struct driver *driver) {
	return driver->bg_colors;
}

const struct charset *driver_get_charset(const struct driver *driver) {
	return driver->charset;
}

void driver_test(const struct driver *driver) {
	g_print(_("Selected output driver: %s (%s)\n"),
			driver_get_name(driver),
			driver_get_description(driver));
	g_print(_("Output drivers supports %u fg and %u bg colors.\n"),
			driver_get_fg_colors(driver),
			driver_get_bg_colors(driver));

	const struct charset *charset = driver_get_charset(driver);
	if (charset == NULL) {
		g_print(_("Driver does not use a charset.\n"));
	} else {
		g_print(_("Selected output charset: %s (%s)\n"),
				charset_get_name(charset),
				charset_get_description(charset));
	}

	const char *charset_name;
	gboolean utf8 = g_get_charset(&charset_name);

	/* The "standard" name for US-ASCII is basically unrecognizable */
	if (strcmp(charset_name, "ANSI_X3.4-1968") == 0)
		charset_name = "US-ASCII";

	if (!utf8) {
		const char *lc_ctype = setlocale(LC_CTYPE, NULL);
		g_print(_("The detected character set is not UTF-8 (it's %s instead).\n"
			"This will result in reduced quality from limited available characters.\n"
			"Try changing your locale LC_CTYPE (it's currently \"%s\").\n\n"),
				charset_name, lc_ctype);
	}
}
