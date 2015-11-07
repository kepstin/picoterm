#ifndef PICOTERM_CHARSET_H
#define PICOTERM_CHARSET_H

#include <stddef.h>

struct charset;

struct glyph {
	const char *code;
	float weights[2][2];
};

enum charset_flags {
	/* Include inverse versions of glyphs, where available.
	 * This is useful for palettes where fg and bg sets are different. */
	CHARSET_INVERSE       = (1 << 0),
	/* Request the "extended" UTF-8 character set, which includes
	 * the quadrant characters for higher resolution. */
	CHARSET_UTF8_EXTENDED = (1 << 1),
	/* Force the character set to initialize, even if it can't determine
	 * whether or not your terminal supports it */
	CHARSET_FORCE         = (1 << 2)
};

const struct glyph **charset_get_glyphs(
		const struct charset *charset,
		size_t *count);
const char *charset_get_enter_string(const struct charset *charset);
const char *charset_get_exit_string(const struct charset *charset);

struct charset *charset_get_default(enum charset_flags flags);
struct charset *charset_get_acs(enum charset_flags flags);
struct charset *charset_get_utf8(enum charset_flags flags);

void charset_free(struct charset *charset);

#endif
