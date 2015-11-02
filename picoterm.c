#include "palette.h"

#include <lcms2.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define SCREEN_SIZE (80 * 80)
static void print_image(const char *filename, const struct palette *palette) {
	FILE *f = fopen(filename, "rb");
	uint8_t *buf = malloc(SCREEN_SIZE * 3);

	size_t pixels = fread(buf, 3, SCREEN_SIZE, f);

	cmsCIELab *lab_buf = malloc(pixels * sizeof(cmsCIELab));

	cmsHPROFILE hsRGB = cmsCreate_sRGBProfile();
	cmsHPROFILE hLab = cmsCreateLab4Profile(cmsD50_xyY());
	cmsHTRANSFORM trans = cmsCreateTransform(hsRGB, TYPE_RGB_8,
			hLab, TYPE_Lab_DBL,
			INTENT_PERCEPTUAL, 0);
	cmsCloseProfile(hsRGB);
	cmsCloseProfile(hLab);
	cmsDoTransform(trans, buf, lab_buf, pixels);
	cmsDeleteTransform(trans);

	uint32_t fg_count = palette->fg_count;
	uint32_t bg_count = palette->bg_count;
	const cmsCIELab *bg_palette = palette->bg_palette();

	char *code = malloc(palette->escape_len);

	for (size_t i = 0; i < pixels; i++) {
		double delta_e = HUGE_VAL;
		uint32_t color = 0;
		for (uint32_t j = 0; j < bg_count; j++) {
			double new_d_e = cmsDeltaE(&lab_buf[i], &bg_palette[j]);
			if (new_d_e < delta_e) {
				delta_e = new_d_e;
				color = j;
			}
		}
		palette->code(code, fg_count, color);
		printf("%s ", code);
		if (i + 1 < pixels) {
			lab_buf[i+1].L += (lab_buf[i].L - bg_palette[color].L) / 2;
			lab_buf[i+1].a += (lab_buf[i].a - bg_palette[color].a) / 2;
			lab_buf[i+1].b += (lab_buf[i].b - bg_palette[color].b) / 2;
		}
		if (i + 79 < pixels) {
			lab_buf[i+79].L += (lab_buf[i].L - bg_palette[color].L) / 4;
			lab_buf[i+79].a += (lab_buf[i].a - bg_palette[color].a) / 4;
			lab_buf[i+79].b += (lab_buf[i].b - bg_palette[color].b) / 4;
		}
		if (i + 80 < pixels) {
			lab_buf[i+80].L += (lab_buf[i].L - bg_palette[color].L) / 4;
			lab_buf[i+80].a += (lab_buf[i].a - bg_palette[color].a) / 4;
			lab_buf[i+80].b += (lab_buf[i].b - bg_palette[color].b) / 4;
		}
	}
	palette->reset(code);
	printf("%s\n", code);
}

int main(int argc, char *argv[]) {
	struct palette *palette = &palette_256color;

	uint32_t fg_count = palette->fg_count;
	const cmsCIELab *fg_palette = palette->fg_palette();
	uint32_t bg_count = palette->bg_count;
	const cmsCIELab *bg_palette = palette->bg_palette();

	uint32_t count = fg_count > bg_count ? fg_count : bg_count;

	char *code = malloc(palette->escape_len);
	char *reset = malloc(palette->escape_len);
	palette->reset(reset);

	for (unsigned i = 0; i < count; i++) {
		if (i < fg_count) {
			palette->code(code, i, bg_count);
			printf("%3u %sXX%s %8.3f %8.3f %8.3f ",
					i, code, reset,
					fg_palette[i].L,
					fg_palette[i].a,
					fg_palette[i].b);
		} else {
			printf("                                   ");
		}
		if (i < bg_count) {
			palette->code(code, fg_count, i);
			printf("%3u %s  %s %8.3f %8.3f %8.3f ",
					i, code, reset,
					bg_palette[i].L,
					bg_palette[i].a,
					bg_palette[i].b);
		}
		printf("\n");
	}

	palette->code(code, 108, 138);
	printf("%s\342\226\200%s", code, reset);
	palette->code(code, 138, 108);
	printf("%s\342\226\200%s", code, reset);
	printf("\n");

	if (argc > 1) {
		print_image(argv[1], palette);
	}

	return 0;
}
