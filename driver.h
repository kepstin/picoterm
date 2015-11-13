#ifndef PICOTERM_DRIVER_H
#define PICOTERM_DRIVER_H

#include <glib.h>

enum driver_resolution {
	DRIVER_RES_FULL = 0,
	DRIVER_RES_HALF,
	DRIVER_RES_QUARTER
};

/* Options to include in the application's command-line handling */
GOptionGroup *driver_get_option_group(void);

/* Load the driver selected based on the command-line options and what
 * can be autodetected (from terminfo, etc.) */
struct driver *driver_get_default(void);

/* Free the memory used by a driver, including internal allocations,
 * charset, palette, etc. */
void driver_free(struct driver *driver);

/* Get the name of the driver */
const char *driver_get_name(const struct driver *driver);

/* Get the description of the driver (translated) */
const char *driver_get_description(const struct driver *driver);

/* Get the number of foreground colors supported */
guint32 driver_get_fg_colors(const struct driver *driver);

/* Get the number of background colors supported */
guint32 driver_get_bg_colors(const struct driver *driver);

/* Get the charset that the driver is using */
const struct charset *driver_get_charset(const struct driver *driver);

/* Get the palette that the driver is using */
const struct palette *driver_get_palette(const struct driver *driver);

/* Get the max number of pixels wide and tall that the driver is capable of
 * rendering */
void driver_get_max_image_size(const struct driver *driver,
		guint *columns, guint *rows);

/* Render an image. The image buffer must be in a packed R8,G8,B8 format. */
void driver_render(const struct driver *driver,
		const guint8 *image, guint columns, guint rows);

/* Generate a test screen that a user can use to validate their config.
 * This can include commentary on suggested options, etc. */
void driver_test(const struct driver *driver);

/* Perform early-program initialization required for terminal drivers */
void driver_term_init(void);

#endif
