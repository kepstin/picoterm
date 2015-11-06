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

	char *code = malloc(palette->escape_len);

	size_t cols = 80;
	size_t rows = pixels / cols;
	unsigned row;

	const cmsCIELab *subrow_palette[2] = { palette->fg_palette(), palette->bg_palette() };
	uint16_t subrow_count[2] = { palette->fg_count, palette->bg_count };
	const char *charcode[2] = { "\342\226\200", "\342\226\204" };

	unsigned max_k = palette->fg_count != palette->bg_count ? 2 : 1;

	for (row = 0; row < rows; row += 2) {
		cmsCIELab dither_err[2][2] = { 0 };
		for (unsigned i = 0; i < 80; i++) {
			unsigned char_i = 0;
			uint16_t color[2][2] = {
				{ subrow_count[0], subrow_count[1] },
				{ subrow_count[1], subrow_count[0] }
			};
			double char_de = HUGE_VAL;
			for (unsigned k = 0; k < max_k; k++) {
				double this_de = 0;
				for (unsigned subrow = 0; subrow < 2 && row + subrow < rows; subrow++) {
					unsigned r = (subrow + k) % 2;
					double de = HUGE_VAL;
					size_t pixel = (row + subrow) * 80 + i;
					const cmsCIELab *p = subrow_palette[r];
					uint16_t count = subrow_count[r];
					uint16_t c = color[k][r];
					cmsCIELab lab_pix = lab_buf[pixel];

					if (subrow > 0) {
						lab_pix.L += dither_err[k][subrow-1].L / 4;
						lab_pix.a += dither_err[k][subrow-1].a / 4;
						lab_pix.b += dither_err[k][subrow-1].b / 4;
					}

					for (uint16_t j = 0; j < count; j++) {
						double new_de = cmsCIE94DeltaE(&lab_pix, &p[j]);
						if (new_de < de) {
							de = new_de;
							c = j;
						}
					}
					this_de += de;
					color[k][r] = c;
					dither_err[k][subrow].L = (lab_buf[pixel].L - p[c].L);
					dither_err[k][subrow].a = (lab_buf[pixel].a - p[c].a);
					dither_err[k][subrow].b = (lab_buf[pixel].b - p[c].b);
				}
				if (this_de < char_de) {
					char_de = this_de;
					char_i = k;
				}
			}

			/* Apply the resulting dither errors */
			if (i + 1 < cols) {
				lab_buf[row * cols + i + 1].L += dither_err[char_i][0].L / 2;
				lab_buf[row * cols + i + 1].a += dither_err[char_i][0].a / 2;
				lab_buf[row * cols + i + 1].b += dither_err[char_i][0].b / 2;
				if (row + 1 < rows) {
					lab_buf[(row + 1) * cols + i + 1].L += dither_err[char_i][1].L / 2;
					lab_buf[(row + 1) * cols + i + 1].a += dither_err[char_i][1].a / 2;
					lab_buf[(row + 1) * cols + i + 1].b += dither_err[char_i][1].b / 2;
				}
			}
			if (row + 2 < rows && i > 0) {
				lab_buf[(row + 2) * cols + i - 1].L += dither_err[char_i][0].L / 4;
				lab_buf[(row + 2) * cols + i - 1].a += dither_err[char_i][0].a / 4;
				lab_buf[(row + 2) * cols + i - 1].b += dither_err[char_i][0].b / 4;
				if (row + 2 < rows) {
					lab_buf[(row + 2) * cols + i - 1].L += dither_err[char_i][1].L / 4;
					lab_buf[(row + 2) * cols + i - 1].a += dither_err[char_i][1].a / 4;
					lab_buf[(row + 2) * cols + i - 1].b += dither_err[char_i][1].b / 4;
				}
			}
			if (row + 2 < rows) {
				lab_buf[(row + 2) * cols + i].L += dither_err[char_i][1].L / 4;
				lab_buf[(row + 2) * cols + i].a += dither_err[char_i][1].a / 4;
				lab_buf[(row + 2) * cols + i].b += dither_err[char_i][1].b / 4;
				/*
				lab_buf[(row + 2) * cols + i].L += dither_err[char_i][0].L / 4;
				lab_buf[(row + 2) * cols + i].a += dither_err[char_i][0].a / 4;
				lab_buf[(row + 2) * cols + i].b += dither_err[char_i][0].b / 4;
				if (row + 3 < rows) {
					lab_buf[(row + 3) * cols + i].L += dither_err[char_i][1].L / 4;
					lab_buf[(row + 3) * cols + i].a += dither_err[char_i][1].a / 4;
					lab_buf[(row + 3) * cols + i].b += dither_err[char_i][1].b / 4;
				}*/
			}

			palette->code(code, color[char_i][0], color[char_i][1]);
			printf("%s%s", code, charcode[char_i]);
		}
		palette->reset(code);
		printf("%s\n", code);
	}
}

static void print_image_truecolor(const char *filename) {
	FILE *f = fopen(filename, "rb");
	uint8_t *buf = malloc(SCREEN_SIZE * 3);

	size_t pixels = fread(buf, 3, SCREEN_SIZE, f);
	size_t rows = pixels / 80;
	unsigned row;

	for (row = 0; row < rows-1; row += 2) {
		for (unsigned i = 0; i < 80; i++) {
			printf("\e[38;2;%u;%u;%um\e[48;2;%u;%u;%um\342\226\200",
				(unsigned) buf[row*80*3+i*3],
				(unsigned) buf[row*80*3+i*3+1],
				(unsigned) buf[row*80*3+i*3+2],
				(unsigned) buf[(row+1)*80*3+i*3],
				(unsigned) buf[(row+1)*80*3+i*3+1],
				(unsigned) buf[(row+1)*80*3+i*3+2]);
		}
		printf("\e[0m\n");
	}
	if (row < rows) {
		for (unsigned i = 0; i < 80; i++) {
			printf("\e[38;2;%u;%u;%um\342\226\200",
				(unsigned) buf[row*80*3+i*3],
				(unsigned) buf[row*80*3+i*3+1],
				(unsigned) buf[row*80*3+i*3+2]);
		}
		printf("\e[0m\n");
	}
}

int main(int argc, char *argv[]) {
	struct palette *palette = &palette_linux;

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
			printf("%3u %s██%s %8.3f %8.3f %8.3f ",
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
		print_image_truecolor(argv[1]);
		print_image(argv[1], palette);
	}

	return 0;
}
