#ifndef PICOTERM_CHARSET_H
#define PICOTERM_CHARSET_H

#include <stddef.h>

struct glyph {
	const char *code;
	float weights[2][2];
};

enum charset_flags {
	CHARSET_INVERSE       = (1 << 0),
	CHARSET_UTF8_EXTENDED = (1 << 1)
};

const struct glyph **charset_acs(size_t *count, enum charset_flags flags);
const struct glyph **charset_utf8(size_t *count, enum charset_flags flags);

#endif
