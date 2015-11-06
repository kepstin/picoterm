#include "charset.h"

#include <stdlib.h>
#include <string.h>

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

/* The set of UTF-8 characters with maximum compatibility.
 * These characters are all required in WGL-4, so they're common in fonts. */
static const struct glyph const *utf8_set_basic[] = {
	&utf8_space,
	&utf8_medium_shade,
	&utf8_dark_shade,
	&utf8_lower_half_block,
	&utf8_left_half_block
};
#define UTF8_SET_BASIC_LEN \
	(sizeof(utf8_set_basic) / sizeof(struct glyph *))

/* Additional UTF-8 maximum compatibility characters for terminals
 * where the colors available for fg vs bg are different.
 * Includes some codes that are optional in WGL-4. */
static const struct glyph const *utf8_set_basic_inv[] = {
	&utf8_full_block,
	&utf8_light_shade,
	&utf8_upper_half_block,
	&utf8_right_half_block
};
#define UTF8_SET_BASIC_INV_LEN \
	(sizeof(utf8_set_basic_inv) / sizeof(struct glyph *))

/* Additional UTF-8 characters for quadrant addressing.
 * Not in all fonts, so their usage is optional */
static const struct glyph const *utf8_set_extended[] = {
	&utf8_quadrant_ul,
	&utf8_quadrant_ur,
	&utf8_quadrant_ll,
	&utf8_quadrant_lr,
	&utf8_quadrant_ul_lr
};
#define UTF8_SET_EXTENDED_LEN \
	(sizeof(utf8_set_extended) / sizeof(struct glyph *))

/* Inverse versions of the quadrant addressing characters. */
static const struct glyph const *utf8_set_extended_inv[] = {
	&utf8_quadrant_ur_ll_lr,
	&utf8_quadrant_ul_ll_lr,
	&utf8_quadrant_ul_ur_lr,
	&utf8_quadrant_ul_ur_ll,
	&utf8_quadrant_ur_ll
};
#define UTF8_SET_EXTENDED_INV_LEN \
	(sizeof(utf8_set_extended_inv) / sizeof(struct glyph *))

/* Build and return a charset */
const struct glyph **charset_utf8(size_t *countp, enum charset_flags flags) {
	size_t count = 0;

	count += UTF8_SET_BASIC_LEN;
	if (flags & CHARSET_INVERSE)
		count += UTF8_SET_BASIC_INV_LEN;
	
	if (flags & CHARSET_UTF8_EXTENDED) {
		count += UTF8_SET_EXTENDED_LEN;
		if (flags & CHARSET_INVERSE)
			count += UTF8_SET_EXTENDED_INV_LEN;
	}

	const struct glyph **charset = malloc(count * sizeof(struct glyph *));

	count = 0;

	memcpy(&charset[count], utf8_set_basic,
		sizeof(utf8_set_basic));
	count += UTF8_SET_BASIC_LEN;
	if (flags & CHARSET_INVERSE) {
		memcpy(&charset[count], utf8_set_basic_inv,
			sizeof(utf8_set_basic_inv));
		count += UTF8_SET_BASIC_INV_LEN;
	}

	if (flags & CHARSET_UTF8_EXTENDED) {
		memcpy(&charset[count], utf8_set_extended,
			sizeof(utf8_set_extended));
		count += UTF8_SET_EXTENDED_LEN;
		if (flags & CHARSET_INVERSE) {
			memcpy(&charset[count], utf8_set_extended_inv,
				sizeof(utf8_set_extended_inv));
			count += UTF8_SET_EXTENDED_INV_LEN;
		}
	}

	*countp = count;
	return charset;
}
