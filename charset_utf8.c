#include "charset.h"
#include "charset_internal.h"

#include <stdlib.h>
#include <string.h>

#include <glib/gi18n.h>

/* U+0020 SPACE - full character box of background color */
static const struct glyph utf8_space = {
	.code = "\x20",
	.weights = { { 0.00, 0.00 }, { 0.00, 0.00 } }
};
/* U+2580 UPPER HALF BLOCK */
static const struct glyph utf8_upper_half_block = {
	.code = "\xE2\x96\x80",
	.weights = { { 1.00, 1.00 }, { 0.00, 0.00 } }
};
/* U+2584 LOWER HALF BLOCK */
static const struct glyph utf8_lower_half_block = {
	.code = "\xE2\x96\x84",
	.weights = { { 0.00, 0.00 }, { 1.00, 1.00 } }
};
/* U+2588 FULL BLOCK */
static const struct glyph utf8_full_block = {
	.code = "\xE2\x96\x88",
	.weights = { { 1.00, 1.00 }, { 1.00, 1.00 } }
};
/* U+258C LEFT HALF BLOCK */
static const struct glyph utf8_left_half_block = {
	.code = "\xE2\x96\x8C",
	.weights = { { 1.00, 0.00 }, { 1.00, 0.00 } }
};
/* U+2590 RIGHT HALF BLOCK */
static const struct glyph utf8_right_half_block = {
	.code = "\xE2\x96\x90",
	.weights = { { 0.00, 0.00 }, { 1.00, 1.00 } }
};
/* U+2591 LIGHT SHADE - 25% foreground */
static const struct glyph utf8_light_shade = {
	.code = "\xE2\x96\x91",
	.weights = { { 0.25, 0.25 }, { 0.25, 0.25 } }
};
/* U+2592 MEDIUM SHADE - 50% foreground */
static const struct glyph utf8_medium_shade = {
	.code = "\xE2\x96\x92",
	.weights = { { 0.50, 0.50 }, { 0.50, 0.50 } }
};
/* U+2593 DARK SHADE - 75% foreground */
static const struct glyph utf8_dark_shade = {
	.code = "\xE2\x96\x93",
	.weights = { { 0.75, 0.57 }, { 0.75, 0.75 } }
};
/* U+2596 QUADRANT LOWER LEFT */
static const struct glyph utf8_quadrant_ll = {
	.code = "\xE2\x96\x96",
	.weights = { { 0.00, 0.00 }, { 1.00, 0.00 } }
};
/* U+2597 QUADRANT LOWER RIGHT */
static const struct glyph utf8_quadrant_lr = {
	.code = "\xE2\x96\x97",
	.weights = { { 0.00, 0.00 }, { 0.00, 1.00 } }
};
/* U+2598 QUADRANT UPPER LEFT */
static const struct glyph utf8_quadrant_ul = {
	.code = "\xE2\x96\x98",
	.weights = { { 1.00, 0.00 }, { 0.00, 0.00 } }
};
/* U+2599 QUADRAND UPPER LEFT AND LOWER LEFT AND LOWER RIGHT */
static const struct glyph utf8_quadrant_ul_ll_lr = {
	.code = "\xE2\x96\x99",
	.weights = { { 1.00, 0.00 }, { 1.00, 1.00 } }
};
/* U+259A QUADRANT UPPER LEFT AND LOWER RIGHT */
static const struct glyph utf8_quadrant_ul_lr = {
	.code = "\xE2\x96\x9B",
	.weights = { { 1.00, 0.00 }, { 0.00, 1.00 } }
};
/* U+259B QUADRANT UPPER LEFT AND UPPER RIGHT AND LOWER LEFT */
static const struct glyph utf8_quadrant_ul_ur_ll = {
	.code = "\xE2\x96\x9B",
	.weights = { { 1.00, 1.00 }, { 1.00, 0.00 } }
};
/* U+259C QUADRANT UPPER LEFT AND UPPER RIGHT AND LOWER RIGHT */
static const struct glyph utf8_quadrant_ul_ur_lr = {
	.code = "\xE2\x96\x9C",
	.weights = { { 1.00, 1.00 }, { 0.00, 1.00 } }
};
/* U+259D QUADRANT UPPER RIGHT */
static const struct glyph utf8_quadrant_ur = {
	.code = "\xE2\x96\x9D",
	.weights = { { 0.00, 1.00 }, { 0.00, 0.00 } }
};
/* U+259E QUADRANT UPPER RIGHT AND LOWER LEFT */
static const struct glyph utf8_quadrant_ur_ll = {
	.code = "\xE2\x96\x9E",
	.weights = { { 0.00, 1.00 }, { 1.00, 0.00 } }
};
/* U+259F QUADRANT UPPER RIGHT AND LOWER LEFT AND LOWER RIGHT */
static const struct glyph utf8_quadrant_ur_ll_lr = {
	.code = "\xE2\x96\x9F",
	.weights = { { 0.00, 1.00 }, { 1.00, 1.00 } }
};

/* Full block characters in the basic (WGL-4) set. */
static const struct glyph const *utf8_set_full[] = {
	&utf8_space
};
#define UTF8_SET_FULL_LEN \
	(sizeof(utf8_set_full) / sizeof(struct glyph *))

/* Inverse block characters in the basic (WGL-4) set.
 * Includes some codes that are listed as optional. */
static const struct glyph const *utf8_set_full_inv[] = {
	&utf8_full_block
};
#define UTF8_SET_FULL_INV_LEN \
	(sizeof(utf8_set_full_inv) / sizeof(struct glyph *))

/* Full block shade characters in the basic (WGL-4) set. */
static const struct glyph const *utf8_set_shade[] = {
	&utf8_dark_shade,
	&utf8_medium_shade
};
#define UTF8_SET_SHADE_LEN \
	(sizeof(utf8_set_shade) / sizeof(struct glyph *))

/* Inverse full block shade characters in the basic (WGL-4) set.
 * Includes some codes that are listed as optional. */
static const struct glyph const *utf8_set_shade_inv[] = {
	&utf8_light_shade
};
#define UTF8_SET_SHADE_INV_LEN \
	(sizeof(utf8_set_shade_inv) / sizeof(struct glyph *))

/* Half block characters in the basic (WGL-4) set. */
static const struct glyph const *utf8_set_half[] = {
	&utf8_lower_half_block
};
#define UTF8_SET_HALF_LEN \
	(sizeof(utf8_set_half) / sizeof(struct glyph *))

/* Inverse half block characters in the basic (WGL-4) set.
 * Includes some codes that are listed as optional. */
static const struct glyph const *utf8_set_half_inv[] = {
	&utf8_upper_half_block
};
#define UTF8_SET_HALF_INV_LEN \
	(sizeof(utf8_set_half_inv) / sizeof(struct glyph *))

/* Quarter block characters.
 * Not enabled by default, since they're often missing from fonts. */
static const struct glyph const *utf8_set_quarter[] = {
	&utf8_left_half_block,
	&utf8_quadrant_ul,
	&utf8_quadrant_ur,
	&utf8_quadrant_ll,
	&utf8_quadrant_lr,
	&utf8_quadrant_ul_lr
};
#define UTF8_SET_QUARTER_LEN \
	(sizeof(utf8_set_quarter) / sizeof(struct glyph *))

/* Inverse versions of the quarter block characters. */
static const struct glyph const *utf8_set_quarter_inv[] = {
	&utf8_right_half_block,
	&utf8_quadrant_ur_ll_lr,
	&utf8_quadrant_ul_ll_lr,
	&utf8_quadrant_ul_ur_lr,
	&utf8_quadrant_ul_ur_ll,
	&utf8_quadrant_ur_ll
};
#define UTF8_SET_QUARTER_INV_LEN \
	(sizeof(utf8_set_quarter_inv) / sizeof(struct glyph *))

/* Build and return a charset */
struct charset *charset_get_utf8(enum charset_flags flags) {
	gboolean utf8 = g_get_charset(NULL);

	if (!utf8 && !(flags & CHARSET_FORCE)) {
		/* Terminal doesn't support UTF-8, bail out. */
		return NULL;
	}

	/* Apply environment quirks */
	// TODO: handle user-passed options

	if (g_getenv("VTE_VERSION") != NULL) {
		/* vte doesn't use fonts for quadrant characters, but actually
		 * draws them internally. We can always enable the extended
		 * charset here. */
		flags |= CHARSET_UTF8_EXTENDED;
	}

	/* Determine the number of glyphs to be included, and the feature
	 * flags we support. */
	size_t count = 0;
	enum charset_flags enabled_flags = flags & CHARSET_INVERSE;

	count += UTF8_SET_FULL_LEN;
	if (flags & CHARSET_INVERSE)
		count += UTF8_SET_FULL_INV_LEN;

	if (flags & CHARSET_SHADE) {
		enabled_flags |= CHARSET_SHADE;
		count += UTF8_SET_SHADE_LEN;
		if (flags & CHARSET_INVERSE)
			count += UTF8_SET_SHADE_INV_LEN;
	}

	if ((flags & CHARSET_RES_HALF) || (flags & CHARSET_RES_QUARTER)) {
		enabled_flags |= CHARSET_RES_HALF;
		count += UTF8_SET_HALF_LEN;
		if (flags & CHARSET_INVERSE)
			count += UTF8_SET_HALF_INV_LEN;
	}

	if ((flags & CHARSET_RES_QUARTER) && (flags & CHARSET_UTF8_EXTENDED)) {
		enabled_flags |= CHARSET_RES_QUARTER;
		count += UTF8_SET_QUARTER_LEN;
		if (flags & CHARSET_INVERSE)
			count += UTF8_SET_QUARTER_INV_LEN;
	}

	struct charset *charset = malloc(sizeof(struct charset));

	charset->name = "utf8";

	charset->description = N_("UTF-8 full-block elements");
	if (enabled_flags & CHARSET_INVERSE) {
		charset->description =
			N_("UTF-8 full-block elements with inverse");
	}
	if (enabled_flags & CHARSET_RES_HALF) {
		charset->description = N_("UTF-8 half-block elements");
		if (enabled_flags & CHARSET_INVERSE) {
			charset->description =
				N_("UTF-8 half-block elements with inverse");
		}
	}
	if (enabled_flags & CHARSET_RES_QUARTER) {
		charset->description = N_("UTF-8 quarter-block elements");
		if (enabled_flags & CHARSET_INVERSE) {
			charset->description =
				N_("UTF-8 quarter-block elements with inverse");
		}
	}

	const struct glyph **glyph = malloc(count * sizeof(struct glyph *));

	count = 0;

	memcpy(&glyph[count], utf8_set_full, sizeof(utf8_set_full));
	count += UTF8_SET_FULL_LEN;

	if (enabled_flags & CHARSET_INVERSE) {
		memcpy(&glyph[count], utf8_set_full_inv,
				sizeof(utf8_set_full_inv));
		count += UTF8_SET_FULL_INV_LEN;
	}

	if (enabled_flags & CHARSET_SHADE) {
		memcpy(&glyph[count], utf8_set_shade, sizeof(utf8_set_shade));
		count += UTF8_SET_SHADE_LEN;

		if (enabled_flags & CHARSET_INVERSE) {
			memcpy(&glyph[count], utf8_set_shade_inv,
					sizeof(utf8_set_shade_inv));
			count += UTF8_SET_SHADE_INV_LEN;
		}
	}

	if (enabled_flags & CHARSET_RES_HALF) {
		memcpy(&glyph[count], utf8_set_half, sizeof(utf8_set_half));
		count += UTF8_SET_HALF_LEN;

		if (enabled_flags & CHARSET_INVERSE) {
			memcpy(&glyph[count], utf8_set_half_inv,
					sizeof(utf8_set_half_inv));
			count += UTF8_SET_HALF_INV_LEN;
		}
	}

	if (enabled_flags & CHARSET_RES_QUARTER) {
		memcpy(&glyph[count], utf8_set_quarter,
				sizeof(utf8_set_quarter));
		count += UTF8_SET_QUARTER_LEN;

		if (enabled_flags & CHARSET_INVERSE) {
			memcpy(&glyph[count], utf8_set_quarter_inv,
				sizeof(utf8_set_quarter_inv));
			count += UTF8_SET_QUARTER_INV_LEN;
		}
	}

	charset->glyph = glyph;

	charset->enter = "";
	charset->exit = "";
	charset->num_glyphs = count;
	charset->flags = enabled_flags;

	return charset;
}
