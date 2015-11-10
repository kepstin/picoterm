#ifndef PICOTERM_CHARSET_H
#define PICOTERM_CHARSET_H

#include <stddef.h>

/* A single glyph within the charset. */
struct glyph {
	/* What to print to the terminal to display this glyph. */
	const char *code;
	/* A 2x2 bitmap describing whether each quadrant of the glyph shows
	 * the foreground (1.0) or background (0.0) or a mix. */
	float weights[2][2];
};

/* Flags used to request particular properties when getting a charset. */
enum charset_flags {
	/* Include inverse versions of glyphs, where available.
	 * This is useful for palettes where fg and bg sets are different. */
	CHARSET_INVERSE       = (1 << 0),
	/* Request the "extended" UTF-8 character set, which includes
	 * additional quadrant characters. */
	CHARSET_UTF8_EXTENDED = (1 << 1),
	/* Force the character set to initialize, even if it can't determine
	 * whether or not your terminal supports it */
	CHARSET_FORCE         = (1 << 2),
	/* Request characters for 1/2 block (top/bottom) resolution. */
	CHARSET_RES_HALF      = (1 << 4),
	/* Request characters for 1/4 block (quadrant) resolution. */
	CHARSET_RES_QUARTER   = (1 << 8),
	/* Include shade/blend characters (usually full-block only). */
	CHARSET_SHADE         = (1 << 9)
};

/* Get the default charset, as detected from environment and terminal
 * capabilities. The only flag that is accepted is CHARSET_INVERSE,
 * others are set only by user configuration. */
struct charset *charset_get_default(enum charset_flags flags);

/* Get a charset that uses the VT10x graphics characters (ACS).
 * Returns NULL if terminfo says ACS isn't supported, unless you use
 * CHARSET_FORCE (in which case it assumes VT10x compatible escape codes). */
struct charset *charset_get_acs(enum charset_flags flags);

/* Get a charset that only contains ASCII SPACE. It should work anywhere,
 * but the drawback is that it can only do background colors. (CHARSET_INVERSE
 * is not supported.)
 * This always succeeds, regardless of whether CHARSET_FORCE is specified. */
struct charset *charset_get_fallback(enum charset_flags flags);

/* Get a charset that uses glyphs from the Unicode Block Element Range.
 * By default it only uses glyphs that are fairly common in fonts, but you
 * can specify CHARSET_UTF8_EXTENDED to get the full quadrant (1/4 box)
 * character set.
 * Returns NULL if the locale isn't using UTF-8, unless you specify
 * CHARSET_FORCE. */
struct charset *charset_get_utf8(enum charset_flags flags);

/* Free the charset structure, including any private (internal) allocations. */
void charset_free(struct charset *charset);

/* Get the name of the charset */
const char *charset_get_name(const struct charset *charset);

/* Get the localized friendly descriptive name of the charset. */
const char *charset_get_description(const struct charset *charset);

/* Get the list of glyphs contained in the charset. */
const struct glyph **charset_get_glyphs(
		const struct charset *charset,
		size_t *count);

/* Get the control string that enters this charset on the terminal.
 * This can be the empty string if no special control string is needed;
 * it will never be NULL. */
const char *charset_get_enter_string(const struct charset *charset);

/* Get the control string that exits this charset on the terminal.
 * This can be the empty string if no special control string is needed;
 * it will never be NULL. */
const char *charset_get_exit_string(const struct charset *charset);

/* Get the flags of features that are supported and enabled in the charset. */
enum charset_flags charset_get_flags(const struct charset *charset);

#endif
