#include "charset.h"
#include "palette.h"

#include <lcms2.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <locale.h>

#include <curses.h>
#include <term.h>

#define QUIRK_SRGB_BLEND FALSE

#define SCREEN_SIZE (80 * 80)
static void print_image(const char *filename, const struct palette *palette) {
	FILE *f = fopen(filename, "rb");
	uint8_t *buf = malloc(SCREEN_SIZE * 3);

	size_t pixels = fread(buf, 3, SCREEN_SIZE, f);

	float *lin_buf = malloc(pixels * 3 * sizeof(float));

	cmsHPROFILE hsRGB = cmsOpenProfileFromFile("sRGB-elle-V4-srgbtrc.icc", "r");
	if (hsRGB == NULL) {
		printf("Couldn't load sRGB icc profile\n");
		return;
	}
	cmsHPROFILE hsRGBlin = cmsOpenProfileFromFile("sRGB-elle-V4-g10.icc", "r");
	if (hsRGBlin == NULL) {
		printf("Couldn't load sRGB linear icc profile\n");
		return;
	}
	cmsHTRANSFORM trans = cmsCreateTransform(hsRGB, TYPE_RGB_8,
			hsRGBlin, TYPE_RGB_FLT,
			INTENT_PERCEPTUAL, 0);
	if (trans == NULL) {
		printf("Couldn't build color-space transform\n");
		return;
	}
	cmsCloseProfile(hsRGB);
	cmsCloseProfile(hsRGBlin);
	cmsDoTransform(trans, buf, lin_buf, pixels);

	uint16_t fg_count = palette->fg_count;
	const guint8 *srgb_fg_palette = palette->fg_palette();
	float *fg_palette = malloc(fg_count * 3 * sizeof(float));
	cmsDoTransform(trans, srgb_fg_palette, fg_palette, fg_count);

	uint16_t bg_count = palette->bg_count;
	const guint8 *srgb_bg_palette = palette->bg_palette();
	float *bg_palette = malloc(bg_count * 3 * sizeof(float));
	cmsDoTransform(trans, srgb_bg_palette, bg_palette, bg_count);

	//enum charset_flags charset_flags = CHARSET_RES_HALF;
	enum charset_flags charset_flags = CHARSET_RES_HALF | CHARSET_SHADE;
	if (fg_count != bg_count)
		charset_flags |= CHARSET_INVERSE;
	struct charset *charset = charset_get_default(charset_flags);
	if (!charset) {
		printf("No charset!\n");
		return;
	}

	size_t glyph_count = 0;
	const struct glyph **glyphs = charset_get_glyphs(charset, &glyph_count);

	char *code = malloc(palette->escape_len);

	size_t cols = 80;
	size_t rows = pixels / cols;
	unsigned row;

	const char *enter_string = charset_get_enter_string(charset);
	const char *exit_string = charset_get_exit_string(charset);
	for (row = 0; row < rows; row += 2) {
		if (enter_string)
			printf("%s", enter_string);
		for (unsigned i = 0; i < 80; i++) {
			const struct glyph *block_glyph = glyphs[3];
			uint16_t block_fg = fg_count;
			uint16_t block_bg = bg_count;
			double block_sq_diff = HUGE_VAL;
			float block_err_top[3] = { 0, 0, 0 };
			float block_err_bot[3] = { 0, 0, 0 };
			gsize top_pos = (row * 80 + i) * 3;
			gsize bot_pos = ((row + 1) * 80 + i) * 3;

			for (size_t glyph_i = 0; glyph_i < glyph_count; glyph_i++) {
				const struct glyph *glyph = glyphs[glyph_i];
				float weight_top = glyph->weights[0][0];
				float weight_bot = glyph->weights[1][0];

				for (uint16_t fg = 0; fg < fg_count; fg++) {
					gsize fg_pos = fg * 3;

					for (uint16_t bg = 0; bg < bg_count; bg++) {
						gsize bg_pos = bg * 3;

						double sq_diff = 0;

						float input_top[3];
						float output_top[3];
						float err_top[3];
						if (QUIRK_SRGB_BLEND) {
							guint8 srgb_output[3] = {
								srgb_fg_palette[fg_pos + 0] * weight_top +
									srgb_bg_palette[bg_pos + 0] * (1 - weight_top),
								srgb_fg_palette[fg_pos + 1] * weight_top +
									srgb_bg_palette[bg_pos + 1] * (1 - weight_top),
								srgb_fg_palette[fg_pos + 2] * weight_top +
									srgb_bg_palette[bg_pos + 2] * (1 - weight_top)
							};
							cmsDoTransform(trans, srgb_output, output_top, 1);
						}
						for (size_t c = 0; c < 3; c++) {
							input_top[c] = lin_buf[top_pos + c];
							if (!QUIRK_SRGB_BLEND)
								output_top[c] = fg_palette[fg_pos + c] * weight_top +
									bg_palette[bg_pos + c] * (1 - weight_top);
							err_top[c] = input_top[c] - output_top[c];
							sq_diff += err_top[c] * err_top[c];
						}

						float input_bot[3] = { 0 };
						float output_bot[3];
						float err_bot[3];
						if (QUIRK_SRGB_BLEND) {
							guint8 srgb_output[3] = {
								srgb_fg_palette[fg_pos + 0] * weight_bot +
									srgb_bg_palette[bg_pos + 0] * (1 - weight_bot),
								srgb_fg_palette[fg_pos + 1] * weight_bot +
									srgb_bg_palette[bg_pos + 1] * (1 - weight_bot),
								srgb_fg_palette[fg_pos + 2] * weight_bot +
									srgb_bg_palette[bg_pos + 2] * (1 - weight_bot)
							};
							cmsDoTransform(trans, srgb_output, output_bot, 1);
						}
						for (size_t c = 0; c < 3; c++) {
							if (row + 1 < rows) {
								input_bot[c] = lin_buf[bot_pos + c];
								input_bot[c] += err_top[c] / 4;
							}
							if (!QUIRK_SRGB_BLEND)
								output_bot[c] = fg_palette[fg_pos + c] * weight_bot +
									bg_palette[bg_pos + c] * (1 - weight_bot);
							err_bot[c] = input_bot[c] - output_bot[c];
							sq_diff += err_bot[c] * err_bot[c];
						}

						if (sq_diff < block_sq_diff) {
							block_glyph = glyph;
							block_fg = fg;
							block_bg = bg;
							block_sq_diff = sq_diff;
							for (size_t c = 0; c < 3; c++) {
								block_err_top[c] = err_top[c];
								block_err_bot[c] = err_bot[c];
							}
						}
					}
				}
			}

			/* Apply the resulting dither errors */
			if (i + 1 < cols) {
				for (size_t c = 0; c < 3; c++)
					lin_buf[top_pos + 1 + c] += block_err_top[c] / 2;
				if (row + 1 < rows) {
					for (size_t c = 0; c < 3; c++)
						lin_buf[bot_pos + 1 + c] += block_err_bot[c] / 2;
				}
			}
			gsize next_pos = ((row + 2) * 80 + i) * 3;
			if (row + 2 < rows && i > 0) {
				for (size_t c = 0; c < 3; c++) {
					lin_buf[next_pos - 1 + c] += block_err_top[c] / 4;
					lin_buf[next_pos - 1 + c] += block_err_bot[c] / 4;
				}
			}
			if (row + 2 < rows) {
				for (size_t c = 0; c < 3; c++)
					lin_buf[next_pos + c] += block_err_bot[c] / 4;
			}

			palette->code(code, block_fg, block_bg);
			printf("%s%s", code, block_glyph->code);
		}
		if (exit_string)
			printf("%s", exit_string);
		palette->reset(code);
		printf("%s\n", code);
	}


	cmsDeleteTransform(trans);
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
	setlocale(LC_ALL, "");

	int err = 0;
	if (setupterm(NULL, 1, &err) == ERR) {
		if (err == -1) {
			printf("The terminfo database could not be found.\n");
		}
		if (err == 0) {
			printf("You are using a generic or unknown setting for TERM,\n");
		}
		printf("You can try manually specifying an output mode.\n");
		exit(1);
	}


	struct palette *palette = &palette_xterm8;

	uint32_t fg_count = palette->fg_count;
	uint32_t bg_count = palette->bg_count;

	uint32_t count = fg_count > bg_count ? fg_count : bg_count;

	char *code = malloc(palette->escape_len);
	char *reset = malloc(palette->escape_len);
	palette->reset(reset);

	for (unsigned i = 0; i < count; i++) {
		if (i < fg_count) {
			palette->code(code, i, bg_count);
			printf("%3u %s██%s ", i, code, reset);
		} else {
			printf("       ");
		}
		if (i < bg_count) {
			palette->code(code, fg_count, i);
			printf("%3u %s  %s ", i, code, reset);
		}
		printf("\n");
	}

	palette->code(code, 108, 138);
	printf("%s\342\226\200%s", code, reset);
	palette->code(code, 138, 108);
	printf("%s\342\226\200%s", code, reset);
	printf("\n");

	if (argc > 1) {
		//print_image_truecolor(argv[1]);
		print_image(argv[1], palette);
	}

	return 0;
}
