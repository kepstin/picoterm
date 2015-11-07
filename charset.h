#ifndef PICOTERM_CHARSET_H
#define PICOTERM_CHARSET_H

#include <stddef.h>

struct charset;

struct glyph {
	const char *code;
	float weights[2][2];
};

enum charset_flags {
	CHARSET_INVERSE       = (1 << 0),
	CHARSET_UTF8_EXTENDED = (1 << 1)
};

const struct glyph **charset_get_glyphs(size_t *count, enum charset_flags flags);
const char *charset_get_enter_string(void);
const char *charset_get_exit_string(void);

struct charset *charset_get_default(enum charset_flags flags);
struct charset *charset_get_acs(enum charset_flags flags);
struct charset *charset_get_utf8(enum charset_flags flags);

#endif
