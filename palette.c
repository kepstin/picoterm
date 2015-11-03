#include "palette.h"

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

const cmsCIELab *palette_convert_srgb_lab(const uint8_t *in, uint16_t entries) {
	cmsCIELab *out = malloc(entries * sizeof(cmsCIELab));

	cmsHPROFILE hsRGB = cmsCreate_sRGBProfile();
	cmsHPROFILE hLab = cmsCreateLab4Profile(cmsD50_xyY());
	cmsHTRANSFORM trans = cmsCreateTransform(hsRGB, TYPE_RGB_8,
			hLab, TYPE_Lab_DBL,
			INTENT_PERCEPTUAL, 0);
	cmsCloseProfile(hsRGB);
	cmsCloseProfile(hLab);

	cmsDoTransform(trans, in, out, entries);

	cmsDeleteTransform(trans);

	return out;
}

void palette_code_16color_bold(char *buf, uint16_t fg, uint16_t bg) {
	bool bold = false;

	if (fg >= 16) {
		fg = 39;
	} else if (fg >= 8) {
		bold = true;
		fg = 30 + (fg - 8);
	} else {
		fg = 30 + fg;
	}

	if (bg >= 8) {
		bg = 49;
	} else {
		bg = 40 + bg;
	}

	if (bold) {
		sprintf(buf, "\e[1;%u;%um", fg, bg);
	} else {
		sprintf(buf, "\e[%u;%um", fg, bg);
	}
}

void palette_code_16color(char *buf, uint16_t fg, uint16_t bg) {
	if (fg >= 16) {
		fg = 39;
	} else if (fg >= 8) {
		fg = 90 + (fg - 8);
	} else {
		fg = 30 + fg;
	}
	if (bg >= 16) {
		bg = 49;
	} else if (bg >= 8) {
		bg = 100 + (bg - 8);
	} else {
		bg = 40 + bg;
	}
	sprintf(buf, "\e[%u;%um", fg, bg);
}

void palette_code_extended(uint16_t count, char *buf, uint16_t fg, uint16_t bg) {
	buf[0] = '\0';

	if (bg >= count || fg >= count) {
		buf += sprintf(buf, "\e[0m");
	}
	if (fg < count) {
		buf += sprintf(buf, "\e[38;5;%um", fg);
	}
	if (bg < count) {
		buf += sprintf(buf, "\e[48;5;%um", bg);
	}

}

void palette_reset(char *buf) {
	strcpy(buf, "\e[0m");
}
