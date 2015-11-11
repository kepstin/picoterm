#include "palette.h"

#define COUNT 16

/* Post-2004 xterm */
static const guint8 const palette[COUNT * 3] = {
	0x00, 0x00, 0x00,
	0xCD, 0x00, 0x00,
	0x00, 0xCD, 0x00,
	0xCD, 0xCD, 0x00,
	0x00, 0x00, 0xEE,
	0xCD, 0x00, 0xCD,
	0x00, 0xCD, 0xCD,
	0xE5, 0xE5, 0xE5,
	0x7F, 0x7F, 0x7F,
	0xFF, 0x00, 0x00,
	0x00, 0xFF, 0x00,
	0xFF, 0xFF, 0x00,
	0x5C, 0x5C, 0xFF,
	0xFF, 0x00, 0xFF,
	0x00, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF
};

static const guint8 *get_palette(void) {
	return palette;
}

struct palette palette_xterm = {
	.name = "xterm",
	.fg_count = COUNT,
	.bg_count = COUNT,
	.fg_palette = get_palette,
	.bg_palette = get_palette,
	.escape_len = PALETTE_CODE_16COLOR_LEN,
	.code = palette_code_16color,
	.reset = palette_reset
};

struct palette palette_xterm8 = {
	.name = "xterm",
	.fg_count = 8,
	.bg_count = 8,
	.fg_palette = get_palette,
	.bg_palette = get_palette,
	.escape_len = PALETTE_CODE_16COLOR_LEN,
	.code = palette_code_16color,
	.reset = palette_reset
};
