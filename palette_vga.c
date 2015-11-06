#include "palette.h"

#include <lcms2.h>

#define COUNT 16

static const uint8_t const palette_srgb[COUNT * 3] = {
	0x00, 0x00, 0x00, /* black */
	0xaa, 0x00, 0x00, /* red */
	0x00, 0xaa, 0x00, /* green */
	0xaa, 0x55, 0x00, /* brown/yellow */
	0x00, 0x00, 0xaa, /* blue */
	0xaa, 0x00, 0xaa, /* magenta */
	0x00, 0xaa, 0xaa, /* cyan */
	0xaa, 0xaa, 0xaa, /* gray */
	0x55, 0x55, 0x55, /* dark gray */
	0xff, 0x55, 0x55, /* bright red */
	0x55, 0xff, 0x55, /* bright green */
	0xff, 0xff, 0x55, /* yellow */
	0x55, 0x55, 0xff, /* bright blue */
	0xff, 0x55, 0xff, /* bright magenta */
	0x55, 0xff, 0xff, /* bright cyan */
	0xff, 0xff, 0xff, /* white */
};

static const cmsCIELab *palette = NULL;

static const cmsCIELab *get_palette(void) {
	if (palette == NULL) {
		palette = palette_convert_srgb_lab(palette_srgb, COUNT);
	}
	return palette;
}

struct palette palette_vga = {
	.name = "vga",
	.fg_count = COUNT,
	.bg_count = COUNT,
	.fg_palette = get_palette,
	.bg_palette = get_palette,
	.escape_len = PALETTE_CODE_16COLOR_LEN,
	.code = palette_code_16color,
	.reset = palette_reset
};

struct palette palette_vga8 = {
	.name = "vga8",
	.fg_count = COUNT,
	.bg_count = COUNT/2,
	.fg_palette = get_palette,
	.bg_palette = get_palette,
	.escape_len = PALETTE_CODE_16COLOR_BOLD_BLINK_LEN,
	.code = palette_code_16color_bold_blink,
	.reset = palette_reset
};

struct palette palette_linux = {
	.name = "linux",
	.fg_count = COUNT,
	.bg_count = COUNT,
	.fg_palette = get_palette,
	.bg_palette = get_palette,
	.escape_len = PALETTE_CODE_16COLOR_BOLD_BLINK_LEN,
	.code = palette_code_16color_bold_blink,
	.reset = palette_reset
};

