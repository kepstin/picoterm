#include "driver_internal.h"
#include "charset.h"
#include "palette.h"

#include <locale.h>
#include <glib/gi18n.h>
#include <stdlib.h>

struct driver *driver_get_default(void) {
	return driver_get_rgb(TRUE);
}

void driver_free(struct driver *driver) {
	/* Palettes are statically allocated, shouldn't be freed */
	if (driver->charset)
		charset_free(driver->charset);
	g_free(driver);
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

const struct palette *driver_get_palette(const struct driver *driver) {
	return driver->palette;
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
		g_print(_("Available characters:"));
		g_print(" [");
		size_t chars = 0;
		const struct glyph **glyph =
			charset_get_glyphs(charset, &chars);
		driver_term_write(charset_get_enter_string(charset));
		for (size_t i = 0; i < chars; i++)
			driver_term_write(glyph[i]->code);
		driver_term_write(charset_get_exit_string(charset));
		g_print("]\n");
	}

	const struct palette *palette = driver_get_palette(driver);
	if (palette == NULL) {
		g_print(_("Driver does not use a palette.\n"));
	} else {
		g_print(_("Selected palette: %s\n"), palette->name);
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
