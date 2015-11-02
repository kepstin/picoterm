#include "palette.h"

#include <lcms2.h>
#include <string.h>
#include <stdlib.h>

static const uint8_t const vga_normal[8 * 3] = {
	0x00, 0x00, 0x00, /* black */
	0xaa, 0x00, 0x00, /* red */
	0x00, 0xaa, 0x00, /* green */
	0xaa, 0x55, 0x00, /* brown/yellow */
	0x00, 0x00, 0xaa, /* blue */
	0xaa, 0x00, 0xaa, /* magenta */
	0x00, 0xaa, 0xaa, /* cyan */
	0xaa, 0xaa, 0xaa, /* gray */
};

static const uint8_t const vga_bright[8 * 3] = {
	0x55, 0x55, 0x55, /* dark gray */
	0xff, 0x55, 0x55, /* bright red */
	0x55, 0xff, 0x55, /* bright green */
	0xff, 0xff, 0x55, /* yellow */
	0x55, 0x55, 0xff, /* bright blue */
	0xff, 0x55, 0xff, /* bright magenta */
	0x55, 0xff, 0xff, /* bright cyan */
	0xff, 0xff, 0xff, /* white */
};

static const cmsCIELab *fg_palette = NULL;
static const cmsCIELab *bg_palette = NULL;

static void init_palette(void) {
	cmsCIELab *new_fg_palette = malloc(16 * sizeof(cmsCIELab));
	cmsCIELab *new_bg_palette = malloc(8 * sizeof(cmsCIELab));

	cmsHPROFILE hsRGB = cmsCreate_sRGBProfile();
	cmsHPROFILE hLab = cmsCreateLab4Profile(cmsD50_xyY());
	cmsHTRANSFORM trans = cmsCreateTransform(hsRGB, TYPE_RGB_8,
			hLab, TYPE_Lab_DBL,
			INTENT_PERCEPTUAL, 0);
	cmsCloseProfile(hsRGB);
	cmsCloseProfile(hLab);

	cmsDoTransform(trans, vga_normal, new_bg_palette, 8);
	memcpy(new_fg_palette, new_bg_palette, 8 * sizeof(cmsCIELab));
	cmsDoTransform(trans, vga_bright, &new_fg_palette[8], 8);

	cmsDeleteTransform(trans);

	fg_palette = new_fg_palette;
	bg_palette = new_bg_palette;
}

static const cmsCIELab *get_fg_palette(void) {
	if (fg_palette == NULL) init_palette();
	return fg_palette;
}

static const cmsCIELab *get_bg_palette(void) {
	if (bg_palette == NULL) init_palette();
	return bg_palette;
}

struct palette palette_vga = {
	.name = "vga",
	.fg_count = 16,
	.bg_count = 8,
	.fg_palette = get_fg_palette,
	.bg_palette = get_bg_palette,
	.escape_len = 11,
	.code = palette_code_16color_bold,
	.reset = palette_reset
};

