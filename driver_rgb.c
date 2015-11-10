#include "driver_internal.h"
#include "charset.h"

#include <unistd.h>

#include <glib/gi18n.h>

const struct driver const driver_rgb = {
	.name = "rgb",
	.description = N_("Truecolor via RGB color codes"),
	.fg_colors = 256 * 256 * 256,
	.bg_colors = 256 * 256 * 256,
	.palette = NULL,
	.charset = NULL
};

struct driver *driver_get_rgb(gboolean force)
{
	gboolean rgb_supported = FALSE;

	if (force)
		rgb_supported = TRUE;

	if (g_getenv("VTE_VERSION") != NULL) {
		rgb_supported = TRUE;
	}

	// TODO: heuristics for other terminals with full rgb support?
	
	/* Bail out if we don't think the rgb driver is gonna work */
	if (!rgb_supported)
		return NULL;

	struct driver *driver = g_new(struct driver, 1);
	memcpy(driver, &driver_rgb, sizeof(struct driver));

	/* Load up a charset... */
	struct charset *charset = charset_get_default(CHARSET_RES_HALF);
	if (charset_get_flags(charset) & CHARSET_RES_HALF) {
		driver->res = DRIVER_RES_HALF;
	} else {
		driver->res = DRIVER_RES_FULL;
	}

	driver->charset = charset;


	return driver;
}
