#ifndef PICOTERM_CHARSET_INTERNAL_H
#define PICOTERM_CHARSET_INTERNAL_H

#include "charset.h"

#include <glib.h>

struct charset {
	const char *name;
	const char *description;
	const struct glyph **glyph;
	const char *enter;
	const char *exit;
	gsize num_glyphs;
	enum charset_flags flags;
};

#endif
