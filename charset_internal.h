#ifndef PICOTERM_CHARSET_INTERNAL_H
#define PICOTERM_CHARSET_INTERNAL_H

#include "charset.h"

struct charset {
	const char *name;
	const char *description;
	size_t num_glyphs;
	const struct glyph **glyph;
	const char *enter;
	const char *exit;
};

#endif
