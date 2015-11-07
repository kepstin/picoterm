#include "driver.h"
#include "driver_internal.h"

#include <glib/gi18n.h>

const struct driver const driver_rgb = {
	.name = "rgb",
	.description = N_("Truecolor via RGB color codes"),
	.fg_colors = 256 * 256 * 256,
	.bg_colors = 256 * 256 * 256
};
