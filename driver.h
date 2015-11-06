#ifndef PICOTERM_DRIVER_H
#define PICOTERM_DRIVER_H

/* Options to include in the application's command-line handling */
extern const struct poptOption const *driver_opts;

/* Load the driver selected based on the command-line options and what
 * can be autodetected (from terminfo, etc.) */
const struct driver *driver(void);

/* Get the max number of pixels wide and tall that the driver is capable of
 * rendering */
void driver_image_size(const struct driver *driver,
		size_t *columns, size_t *rows);

/* Render an image. The image buffer must be in a packed R8,G8,B8 format. */
void driver_render(const struct driver *driver,
		const uint8_t *image, size_t columns, size_t rows);

/* Generate a test screen that a user can use to validate their config.
 * This can include commentary on suggested options, etc. */
void driver_test(const struct driver *driver);

#endif
