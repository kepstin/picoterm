#include "palette.h"

#include <lcms2.h>

#define COUNT 16

static const cmsCIELab const old_palette[COUNT] = {
	{ .L = 20, .a = -12, .b = -12 }, /* 0 - base03 */
	{ .L = 50, .a =  65, .b =  45 }, /* 1 - red */
	{ .L = 60, .a = -20, .b =  65 }, /* 2 - green */
	{ .L = 60, .a =  10, .b =  65 }, /* 3 - yellow */
	{ .L = 55, .a = -10, .b = -45 }, /* 4 - blue */
	{ .L = 50, .a =  65, .b = -5  }, /* 5 - magenta */
	{ .L = 60, .a = -35, .b = -5  }, /* 6 - cyan */
	{ .L = 92, .a =  0,  .b =  10 }, /* 7 - base2 */
	{ .L = 15, .a = -12, .b = -12 }, /* 8 - base03 */
	{ .L = 50, .a =  50, .b =  55 }, /* 9 - orange */
	{ .L = 45, .a = -7,  .b = -7  }, /* 10 - base01 */
	{ .L = 50, .a = -7,  .b = -7  }, /* 11 - base00 */
	{ .L = 60, .a = -6,  .b = -3  }, /* 12 - base0 */
	{ .L = 50, .a =  15, .b = -45 }, /* 13 - violet */
	{ .L = 65, .a = -5,  .b = -2  }, /* 14 - base1 */
	{ .L = 97, .a =  0,  .b =  10 }, /* 15 - base3 */
};

static const uint8_t const srgb_palette[COUNT * 3] = {
	0x07, 0x36, 0x42,
	0xdc, 0x32, 0x2f,
	0x85, 0x99, 0x00,
	0xb5, 0x89, 0x00,
	0x26, 0x8b, 0xd2,
	0xd3, 0x36, 0x82,
	0x2a, 0xa1, 0x98,
	0xee, 0xe8, 0xd5,
	0x00, 0x2b, 0x36,
	0xcb, 0x4b, 0x16,
	0x58, 0x6e, 0x75,
	0x65, 0x7b, 0x83,
	0x83, 0x94, 0x96,
	0x6c, 0x71, 0xc4,
	0x93, 0xa1, 0xa1,
	0xfd, 0xf6, 0xe3
};

static const cmsCIELab *palette = NULL;

static const cmsCIELab *get_palette(void) {
	if (palette == NULL) {
		palette = palette_convert_srgb_lab(srgb_palette, COUNT);
	}
	return palette;
}

struct palette palette_solarized = {
	.name = "solarized",
	.fg_count = COUNT,
	.bg_count = COUNT,
	.fg_palette = get_palette,
	.bg_palette = get_palette,
	.escape_len = PALETTE_CODE_16COLOR_LEN,
	.code = palette_code_16color,
	.reset = palette_reset
};

