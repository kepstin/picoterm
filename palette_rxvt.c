#include "palette.h"

#define COUNT 16

static const guint8 palette[COUNT * 3] = {
	0x00, 0x00, 0x00,
	0xcd, 0x00, 0x00,
	0x00, 0xcd, 0x00,
	0xcd, 0xcd, 0x00,
	0x00, 0x00, 0xcd,
	0xcd, 0x00, 0xcd,
	0x00, 0xcd, 0xcd,
	0xe5, 0xe5, 0xe5,
	0x40, 0x40, 0x40,
	0xff, 0x00, 0x00,
	0x00, 0xff, 0x00,
	0xff, 0xff, 0x00,
	0x00, 0x00, 0xff,
	0xff, 0x00, 0xff,
	0x00, 0xff, 0xff,
	0xff, 0xff, 0xff
};

static const guint8 *get_palette(void) {
	return palette;
}

struct palette palette_rxvt = {
	.name = "rxvt",
	.fg_count = COUNT,
	.bg_count = COUNT,
	.fg_palette = get_palette,
	.bg_palette = get_palette,
	.escape_len = PALETTE_CODE_16COLOR_LEN,
	.code = palette_code_16color,
	.reset = palette_reset
};

