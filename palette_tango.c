#include "palette.h"

#define COUNT 16

static const guint8 palette[COUNT * 3] = {
	0x00, 0x00, 0x00,
	0xcc, 0x00, 0x00,
	0x4e, 0x9a, 0x06,
	0xc4, 0xa0, 0x00,
	0x34, 0x65, 0xa4,
	0x75, 0x50, 0x7b,
	0x06, 0x98, 0x9a,
	0xd3, 0xd7, 0xcf,
	0x55, 0x57, 0x53,
	0xef, 0x29, 0x29,
	0x8a, 0xe2, 0x34,
	0xfc, 0xe9, 0x4f,
	0x72, 0x9f, 0xcf,
	0xad, 0x7f, 0xa8,
	0x34, 0xe2, 0xe2,
	0xee, 0xee, 0xec
};

static const guint8 *get_palette(void) {
	return palette;
}

struct palette palette_tango = {
	.name = "tango",
	.fg_count = COUNT,
	.bg_count = COUNT,
	.fg_palette = get_palette,
	.bg_palette = get_palette,
	.escape_len = PALETTE_CODE_16COLOR_LEN,
	.code = palette_code_16color,
	.reset = palette_reset
};

