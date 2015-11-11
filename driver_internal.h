#ifndef PICOTERM_DRIVER_INTERNAL_H
#define PICOTERM_DRIVER_INTERNAL_H

#include "driver.h"

struct driver {
	/* Name of the driver. */
	const char *name;
	/* Human-readable (localized) detailed driver name. */
	const char *description;
	/* Number of foreground colors the driver can render. */
	guint32 fg_colors;
	/* Number of background colors the driver can render. */
	guint32 bg_colors;
	/* The palette used, or null if the driver doesn't use a palette. */
	struct palette *palette;
	/* The charset used, or null if the driver doesn't use a charset. */
	struct charset *charset;
	/* The sub-block resolution used when rendering. */
	enum driver_resolution res;
};

struct driver *driver_get_rgb(gboolean force);

/* Shared helper functions for terminfo-based drivers */

/* Get the size of the terminal window/screen. */
void driver_term_get_size(guint32 *columns, guint32 *lines);

/* Write a string, possibly including terminfo control commands, to the
 * terminal in an appropriate way */
void driver_term_write(const char *string);

#endif
