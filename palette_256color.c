#include "palette.h"

#include <stdlib.h>
#include <glib.h>

/* The points used on the color cube */
#define COLOR_COUNT 6
static const guint8 color[COLOR_COUNT] = {
	0x00, 0x5f, 0x87, 0xaf, 0xd7, 0xff
};

/* The points used on the greyscale gradient */
#define GREY_COUNT 24
static const guint8 grey[GREY_COUNT] = {
	0x08, 0x12, 0x1c, 0x26, 0x30, 0x3a,
	0x44, 0x4e, 0x58, 0x60, 0x66, 0x76,
	0x80, 0x8a, 0x94, 0x9e, 0xa8, 0xb2,
	0xbc, 0xc6, 0xd0, 0xda, 0xe4, 0xee
};

/* The number of colors in the palette */
#define COUNT (COLOR_COUNT * COLOR_COUNT * COLOR_COUNT + GREY_COUNT)

/* Convert from palette index to xterm color number */
#define TO_XTERM(i) ((i) + 16)

static const guint8 *palette = NULL;

static const guint8 *get_palette(void) {
	if (palette == NULL) {
		/* Generate the sRGB palette */
		guint8 *sRGB = g_new(guint8, COUNT * 3);
		gsize i = 0;
		for (gsize r = 0; r < COLOR_COUNT; r++) {
			for (gsize g = 0; g < COLOR_COUNT; g++) {
				for (gsize b = 0; b < COLOR_COUNT; b++) {
					sRGB[i++] = color[r];
					sRGB[i++] = color[g];
					sRGB[i++] = color[b];
				}
			}
		}
		for (gsize k = 0; k < GREY_COUNT; k++) {
			sRGB[i++] = grey[k]; /* r */
			sRGB[i++] = grey[k]; /* g */
			sRGB[i++] = grey[k]; /* b */
		}

		palette = sRGB;
	}

	return palette;
}

void code(char *buf, uint16_t fg, uint16_t bg) {
	palette_code_extended(TO_XTERM(COUNT), buf, TO_XTERM(fg), TO_XTERM(bg));
}

struct palette palette_256color = {
	.name = "256color",
	.fg_count = COUNT,
	.bg_count = COUNT,
	.fg_palette = get_palette,
	.bg_palette = get_palette,
	.escape_len = PALETTE_CODE_EXTENDED_LEN,
	.code = code,
	.reset = palette_reset
};
