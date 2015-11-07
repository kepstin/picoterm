#include "driver.h"
#include "driver_internal.h"

#include <locale.h>
#include <glib/gi18n.h>
#include <stdlib.h>

const struct driver *driver_get_default(void) {
	struct driver *driver = malloc(sizeof(struct driver));
	memcpy(driver, &driver_rgb, sizeof(struct driver));
	return driver;
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

void driver_test(const struct driver *driver) {
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

	g_print(_("Selected output driver: %s (%s)\n"),
			driver_get_name(driver),
			driver_get_description(driver));
	g_print(_("Output drivers supports %u fg and %u bg colors.\n"),
			driver_get_fg_colors(driver),
			driver_get_bg_colors(driver));
}
