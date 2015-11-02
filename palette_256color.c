#include "palette.h"

#include <lcms2.h>
#include <stdlib.h>

/* The points used on the color cube */
#define COLOR_COUNT 6
static const uint8_t const color[COLOR_COUNT] = {
	0x00, 0x5f, 0x87, 0xaf, 0xd7, 0xff
};

/* The points used on the greyscale gradient */
#define GREY_COUNT 24
static const uint8_t const grey[GREY_COUNT] = {
	0x08, 0x12, 0x1c, 0x26, 0x30, 0x3a,
	0x44, 0x4e, 0x58, 0x60, 0x66, 0x76,
	0x80, 0x8a, 0x94, 0x9e, 0xa8, 0xb2,
	0xbc, 0xc6, 0xd0, 0xda, 0xe4, 0xee
};

/* The number of colors in the palette */
#define COUNT (COLOR_COUNT * COLOR_COUNT * COLOR_COUNT + GREY_COUNT)

/* Convert from palette index to xterm color number */
#define TO_XTERM(i) ((i) + 16)

static const cmsCIELab *palette = NULL;

static const cmsCIELab *get_palette(void) {
	if (palette != NULL) {
		return palette;
	}

	/* Generate the sRGB palette */
	uint8_t *sRGB = malloc(COUNT * 3 * sizeof(uint8_t));
	unsigned i = 0;
	for (unsigned r = 0; r < COLOR_COUNT; r++) {
		for (unsigned g = 0; g < COLOR_COUNT; g++) {
			for (unsigned b = 0; b < COLOR_COUNT; b++) {
				sRGB[i++] = color[r];
				sRGB[i++] = color[g];
				sRGB[i++] = color[b];
			}
		}
	}
	for (unsigned k = 0; k < GREY_COUNT; k++) {
		sRGB[i++] = grey[k]; /* r */
		sRGB[i++] = grey[k]; /* g */
		sRGB[i++] = grey[k]; /* b */
	}

	/* Convert the palette to CIELab */
	cmsCIELab *Lab = malloc(COUNT * sizeof(cmsCIELab));

	cmsHPROFILE hsRGB = cmsCreate_sRGBProfile();
	cmsHPROFILE hLab = cmsCreateLab4Profile(cmsD50_xyY());
	cmsHTRANSFORM trans = cmsCreateTransform(hsRGB, TYPE_RGB_8,
			hLab, TYPE_Lab_DBL,
			INTENT_PERCEPTUAL, 0);
	cmsCloseProfile(hsRGB);
	cmsCloseProfile(hLab);

	cmsDoTransform(trans, sRGB, Lab, COUNT);

	cmsDeleteTransform(trans);

	free(sRGB);

	palette = Lab;

	return Lab;
}

void code(char *buf, uint32_t fg, uint32_t bg) {
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
