#ifndef PICOTERM_DRIVER_INTERNAL_H
#define PICOTERM_DRIVER_INTERNAL_H

#include "driver.h"

struct driver {
	const char *name;
	const char *description;
	guint32 fg_colors;
	guint32 bg_colors;
	struct palette *palette;
	struct charset *charset;
	enum driver_resolution res;
};

struct driver *driver_get_rgb(gboolean force);

#endif
