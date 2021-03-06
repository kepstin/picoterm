#include "charset.h"
#include "palette.h"
#include "driver.h"

#include <lcms2.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <locale.h>
#include <glib.h>
#include <string.h>


/* Set this to TRUE if you're using gnome-terminal (or any libvte terminal),
 * or FALSE otherwise. The picture will look incorrect if it is set wrong. */
#define QUIRK_SRGB_BLEND TRUE

#define SCREEN_SIZE (80 * 80)
void print_image(const char *filename, const struct palette *palette) {
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

				gboolean srgb_blend = FALSE;
				if (weight_top != 1.0 && weight_top != 0.0 &&
						weight_bot != 1.0 && weight_bot != 0.0 &&
						QUIRK_SRGB_BLEND) {
					srgb_blend = TRUE;
				}


				for (uint16_t fg = 0; fg < fg_count; fg++) {
					gsize fg_pos = fg * 3;

					for (uint16_t bg = 0; bg < bg_count; bg++) {
						gsize bg_pos = bg * 3;

						double sq_diff = 0;

						float input_top[3];
						float output_top[3];
						float err_top[3];
						if (srgb_blend) {
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
							if (!srgb_blend)
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

G_DEFINE_AUTO_CLEANUP_FREE_FUNC(cmsHPROFILE, cmsCloseProfile, NULL)
G_DEFINE_AUTO_CLEANUP_FREE_FUNC(cmsHTRANSFORM, cmsDeleteTransform, NULL)
G_DEFINE_AUTOPTR_CLEANUP_FUNC(FILE, fclose)

typedef float v4f __attribute__ ((vector_size (16)));

void print_image_palette_quarter(const char *filename, const struct palette *palette) {
	g_autoptr(FILE) f = fopen(filename, "rb");

	gsize columns = 160;
	g_autofree guint8 *buf = g_new(guint8, columns * 80 * 3);

	gsize pixels = fread(buf, sizeof(guint8) * 3, columns * 80, f);
	gsize lines = pixels / columns;

	/* icc profile for sRGB, standard sRGB gamma curve */
	g_auto(cmsHPROFILE) hsRGB = cmsOpenProfileFromFile("sRGB-elle-V4-srgbtrc.icc", "r");
	if (hsRGB == NULL) {
		printf("Couldn't load sRGB icc profile\n");
		return;
	}
	/* icc profile for sRGB gamut linear gamma */
	g_auto(cmsHPROFILE) hsRGBlin = cmsOpenProfileFromFile("sRGB-elle-V4-g10.icc", "r");
	if (hsRGBlin == NULL) {
		printf("Couldn't load sRGB linear icc profile\n");
		return;
	}

	/* Forwards transform to working space */
	g_auto(cmsHTRANSFORM) trans = cmsCreateTransform(
			hsRGB, TYPE_RGB_8,
			hsRGBlin, TYPE_RGBA_FLT,
			INTENT_PERCEPTUAL, 0);
	if (trans == NULL) {
		printf("Couldn't build color-space transforms\n");
		return;
	}

	g_autofree v4f (*lin_buf)[columns] =
		g_malloc(sizeof(v4f[columns]) * lines);

	cmsDoTransform(trans, buf, lin_buf, columns * lines);

	enum charset_flags charset_flags = CHARSET_RES_HALF |
		CHARSET_RES_QUARTER | CHARSET_SHADE | CHARSET_UTF8_EXTENDED;
	g_autoptr(charset) charset = charset_get_default(charset_flags);
	if (!charset) {
		printf("No charset!\n");
		return;
	}

	size_t glyph_count = 0;
	const struct glyph **glyphs = charset_get_glyphs(charset, &glyph_count);

	uint16_t fg_count = palette->fg_count;
	const guint8 *srgb_fg_palette = palette->fg_palette();
	g_autofree v4f (*fg_palette) = g_malloc(sizeof(v4f) * fg_count);
	cmsDoTransform(trans, srgb_fg_palette, fg_palette, fg_count);

	uint16_t bg_count = palette->bg_count;
	const guint8 *srgb_bg_palette = palette->bg_palette();
	g_autofree v4f (*bg_palette) = g_malloc(sizeof(v4f) * bg_count);
	cmsDoTransform(trans, srgb_bg_palette, bg_palette, bg_count);

	g_autofree char *code = g_malloc(palette->escape_len);
	const char *enter_string = charset_get_enter_string(charset);
	const char *exit_string = charset_get_exit_string(charset);

	v4f (**blend_srgb)[bg_count] = NULL;
	if ((charset_get_flags(charset) & CHARSET_SHADE) && QUIRK_SRGB_BLEND) {
		g_print("Precalculating sRGB shade LUT...\n");
		/* The colorspace conversion is really slow, so to make
		 * comparisons fast, generated precalculated blend color
		 * tables */
		blend_srgb = g_malloc0(sizeof(v4f *) * glyph_count);

		for (gsize g = 0; g < glyph_count; g++) {
			const struct glyph *glyph = glyphs[g];
			/* For now, assume all blends are full-block */
			gfloat weight = glyph->weights[0][0];
			if (weight >= 1.0 || weight <= 0.0)
				continue;

			blend_srgb[g] = g_malloc(sizeof(v4f[bg_count]) * fg_count);
			for (gsize fg = 0; fg < fg_count; fg++) {
				for (gsize bg = 0; bg < bg_count; bg++) {
					guint8 srgb_color[3];
					/* For now, assume all blends are full-block */
					for (size_t c = 0; c < 3; c++) {
						srgb_color[c] =
							(srgb_fg_palette[fg*3 + c] * glyph->weights[0][0]) +
							(srgb_bg_palette[bg*3 + c] * (1 - glyph->weights[0][0]));
					}
					cmsDoTransform(trans, srgb_color,
							&blend_srgb[g][fg][bg], 1);
				}
			}
		}
		g_print("Done!\n");
	}

	for (gsize line = 0; line < lines; line += 2) {
		if (enter_string)
			printf("%s", enter_string);
		for (gsize column = 0; column < columns; column += 2) {
			v4f block[2][2] = { { { 0 } } };
			for (gsize l = 0; l < 2; l++) {
				if (line + l >= lines)
					continue;
				for (gsize c = 0; c < 2; c++) {
					if (column + c >= columns)
						continue;
					block[l][c] = lin_buf[line + l][column + c];
				}
			}

			guint16 block_fg = fg_count;
			guint16 block_bg = bg_count;
			const struct glyph *block_glyph = glyphs[0];
			gdouble block_diff_sq = HUGE_VAL;
			v4f block_err[2][2] = { { { 0 } } };

			for (gsize g = 0; g < glyph_count; g++) {
				const struct glyph *glyph = glyphs[g];
				/* Test the available palette colors */
				for (gsize fg = 0; fg < fg_count; fg++) {
					v4f fg_color = fg_palette[fg];
					for (gsize bg = 0; bg < bg_count; bg++) {
						v4f bg_color = bg_palette[bg];

						v4f err[2][2] = { { { 0 } } };
						double diff_sq = 0;
						/* Go through the sub-blocks, calculating
						 * their real color and error values */
						for (gsize l = 0; l < 2; l++) {
							for (gsize c = 0; c < 2; c++) {
								v4f orig_color = block[l][c];
								/* Apply dithering */
								if (c > 0) {
									orig_color += err[l][c - 1] / 2;
								}
								if (l > 0) {
									if (c < 1) {
										orig_color += err[l - 1][c + 1] / 4;
										orig_color += err[l - 1][c] / 4;
									} else {
										orig_color += err[l - 1][c] / 2;
									}
								}

								v4f color;
								if (blend_srgb && blend_srgb[g]) {
									color = blend_srgb[g][fg][bg];
								} else {
									color = fg_color * glyph->weights[l][c] +
										bg_color * (1 - glyph->weights[l][c]);
								}
								err[l][c] = orig_color - color;
								v4f err_sq = err[l][c] * err[l][c];
								diff_sq += err_sq[0] + err_sq[1] + err_sq[2];
							}
						}

						if (diff_sq < block_diff_sq) {
							block_fg = fg;
							block_bg = bg;
							block_glyph = glyph;
							block_diff_sq = diff_sq;
							memcpy(block_err, err, sizeof(v4f[2][2]));
						}
					}
				}
			}

			/* Apply the resulting dither errors */
			if (column + 2 < columns) {
				lin_buf[line][column + 2] += block_err[0][1] / 2;
				if (line + 1 < lines) {
					lin_buf[line + 1][column + 2] += block_err[1][1] / 2;
				}
			}
			if (line + 2 < lines) {
				if (column > 0) {
					lin_buf[line + 2][column - 1] += block_err[1][0] / 4;
					lin_buf[line + 2][column] += block_err[1][0] / 4;
				} else {
					lin_buf[line + 2][column] += block_err[1][0] / 2;
				}
				if (column + 1 < columns) {
					lin_buf[line + 2][column] += block_err[1][1] / 4;
					lin_buf[line + 2][column + 1] += block_err[1][1] / 4;
				}
			}

			palette->code(code, block_fg, block_bg);
			printf("%s%s", code, block_glyph->code);
		}
		if (exit_string)
			printf("%s", exit_string);
		palette->reset(code);
		printf("%s\n", code);
	}

	if (blend_srgb) {
		for (gsize g = 0; g < glyph_count; g++)
			g_free(blend_srgb[g]);
	}
	g_free(blend_srgb);
}

void print_image_truecolor(const char *filename) {
	g_autoptr(FILE) f = fopen(filename, "rb");

	gsize columns = 160;
	g_autofree guint8 *buf = g_new(guint8, columns * 80 * 3);

	gsize pixels = fread(buf, sizeof(guint8) * 3, columns * 80, f);
	gsize lines = pixels / columns;

	/* icc profile for sRGB, standard sRGB gamma curve */
	g_auto(cmsHPROFILE) hsRGB = cmsOpenProfileFromFile("sRGB-elle-V4-srgbtrc.icc", "r");
	if (hsRGB == NULL) {
		printf("Couldn't load sRGB icc profile\n");
		return;
	}
	/* icc profile for sRGB gamut linear gamma */
	g_auto(cmsHPROFILE) hsRGBlin = cmsOpenProfileFromFile("sRGB-elle-V4-g10.icc", "r");
	if (hsRGBlin == NULL) {
		printf("Couldn't load sRGB linear icc profile\n");
		return;
	}

	/* Forwards transform to working space */
	g_auto(cmsHTRANSFORM) trans = cmsCreateTransform(
			hsRGB, TYPE_RGB_8,
			hsRGBlin, TYPE_RGB_FLT,
			INTENT_PERCEPTUAL, 0);
	/* Reverse transform back to display space */
	g_auto(cmsHTRANSFORM) inv_trans = cmsCreateTransform(
			hsRGBlin, TYPE_RGB_FLT,
			hsRGB, TYPE_RGB_8,
			INTENT_PERCEPTUAL, 0);
	if (trans == NULL || inv_trans == NULL) {
		printf("Couldn't build color-space transforms\n");
		return;
	}

	g_autofree gfloat (*lin_buf)[columns][3] =
		g_malloc(sizeof(gfloat[columns][3]) * lines);

	cmsDoTransform(trans, buf, lin_buf, columns * lines);

	enum charset_flags charset_flags = CHARSET_RES_HALF |
		CHARSET_RES_QUARTER | CHARSET_UTF8_EXTENDED;
	g_autoptr(charset) charset = charset_get_default(charset_flags);
	if (!charset) {
		printf("No charset!\n");
		return;
	}

	size_t glyph_count = 0;
	const struct glyph **glyphs = charset_get_glyphs(charset, &glyph_count);

	for (gsize line = 0; line < lines; line += 2) {
		for (gsize column = 0; column < columns; column += 2) {
			gfloat block[2][2][3] = {
				{ { 0, 0, 0 }, { 0, 0, 0 } },
				{ { 0, 0, 0 }, { 0, 0, 0 } }
			};

			for (gsize l = 0; l < 2; l++) {
				if (line + l >= lines)
					continue;

				for (gsize c = 0; c < 2; c++) {
					if (column + c >= columns)
						continue;

					memcpy(&block[l][c],
						&lin_buf[line + l][column + c],
						sizeof (gfloat[3]));
				}
			}

			gfloat block_fg[3] = { 0, 0, 0 };
			gfloat block_bg[3] = { 0, 0, 0 };
			const struct glyph *block_glyph = glyphs[0];
			double block_err = HUGE_VAL;
			gfloat block_out[2][2][3] = {{{ 0 }}};

			for (gsize g = 0; g < glyph_count; g++) {
				const struct glyph *glyph = glyphs[g];
				/* Calculate the fg/bg colors that this glyph
				 * would use */
				gfloat fg[3] = { 0, 0, 0 };
				gfloat fg_weight = 0;
				gfloat bg[3] = { 0, 0, 0 };
				gfloat bg_weight = 0;
				for (gsize l = 0; l < 2; l++) {
					for (gsize c = 0; c < 2; c++) {
						for (gsize i = 0; i < 3; i++) {
							fg[i] += block[l][c][i] * glyph->weights[l][c];
						}
						fg_weight += glyph->weights[l][c];
						for (gsize i = 0; i < 3; i++) {
							bg[i] += block[l][c][i] * (1 - glyph->weights[l][c]);
						}
						bg_weight += (1 - glyph->weights[l][c]);
					}
				}
				if (fg_weight > 0) {
					for (gsize i = 0; i < 3; i++)
						fg[i] /= fg_weight;
				} else {
					fg[0] = 0;
					fg[1] = 0;
					fg[2] = 0;
				}
				if (bg_weight > 0) {
					for (gsize i = 0; i < 3; i++)
						bg[i] /= bg_weight;
				} else {
					bg[0] = 0;
					bg[1] = 0;
					bg[2] = 0;
				}
				/* Calculate the error */
				double err = 0;
				gfloat out[2][2][3] = {{{ 0 }}};
				for (gsize l = 0; l < 2; l++) {
					for (gsize c = 0; c < 2; c++) {
						for (gsize i = 0; i < 3; i++) {
							out[l][c][i] = 
								fg[i] * glyph->weights[l][c] +
								bg[i] * (1 - glyph->weights[l][c]);
							double diff = block[l][c][i] - out[l][c][i];
							err += diff * diff;
						}
					}
				}
				/* Use this glyph if it has better error */
				if (err < block_err) {
					memcpy(block_fg, fg, sizeof(gfloat[3]));
					memcpy(block_bg, bg, sizeof(gfloat[3]));
					memcpy(block_out, out, sizeof(gfloat[2][2][3]));
					block_glyph = glyph;
					block_err = err;
				}
			}

			/* Convert the colors back to sRGB */
			guint8 fg_srgb[3];
			guint8 bg_srgb[3];
			cmsDoTransform(inv_trans, block_fg, fg_srgb, 1);
			cmsDoTransform(inv_trans, block_bg, bg_srgb, 1);

			/* Print the glyph */
			printf("\e[38;2;%u;%u;%um\e[48;2;%u;%u;%um%s",
				(unsigned) fg_srgb[0],
				(unsigned) fg_srgb[1],
				(unsigned) fg_srgb[2],
				(unsigned) bg_srgb[0],
				(unsigned) bg_srgb[1],
				(unsigned) bg_srgb[2],
				block_glyph->code);
		}
		printf("\e[0m\n");
	}
}

int main(int argc, char *argv[]) {
	setlocale(LC_ALL, "");

	driver_term_init();

	struct palette *palette = &palette_tango;

	uint32_t fg_count = palette->fg_count;
	uint32_t bg_count = palette->bg_count;

	uint32_t count = fg_count > bg_count ? fg_count : bg_count;

	g_autofree char *code = g_malloc(palette->escape_len);
	g_autofree char *reset = g_malloc(palette->escape_len);
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
		print_image_truecolor(argv[1]);
		//print_image(argv[1], palette);
		//print_image_palette_quarter(argv[1], palette);
	}

	return 0;
}
