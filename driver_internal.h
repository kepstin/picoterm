#ifndef PICOTERM_DRIVER_INTERNAL_H
#define PICOTERM_DRIVER_INTERNAL_H

struct driver {
	const char *name;
	const char *description;
	guint32 fg_colors;
	guint32 bg_colors;
	struct charset *charset;
};

extern const struct driver const driver_rgb;

#endif
