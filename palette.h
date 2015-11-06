#ifndef PICOTERM_PALETTE_H
#define PICOTERM_PALETTE_H

#include <lcms2.h>
#include <stdint.h>

struct palette {
	const char *name;
	uint16_t fg_count;
	uint16_t bg_count;
	const cmsCIELab *(* fg_palette)(void);
	const cmsCIELab *(* bg_palette)(void);
	size_t escape_len;
	void (* code)(char *buf, uint16_t fg, uint16_t bg);
	void (* reset)(char *buf);
};

extern struct palette palette_256color;
extern struct palette palette_linux; /* vga with bold/blink */
extern struct palette palette_rxvt;
extern struct palette palette_solarized;
extern struct palette palette_tango;
extern struct palette palette_vga;
extern struct palette palette_vga8;

/* Shared helper functions used by multiple palettes */

/* Convert a palette from 8 bit per component R, G, B to CIELab */
const cmsCIELab *palette_convert_srgb_lab(const uint8_t *in, uint16_t entries);

/* For terminals that support 16 colors, but select the bright fg colors with
 * bold, and bright background colors with blink.
 */
#define PALETTE_CODE_16COLOR_BOLD_BLINK_LEN 15
void palette_code_16color_bold_blink(char *buf, uint16_t fg, uint16_t bg);

/* For terminals that support addressing 16 colors independent of bold */
#define PALETTE_CODE_16COLOR_LEN 10
void palette_code_16color(char *buf, uint16_t fg, uint16_t bg);

/* For terminals that support addressing up to 256 colors via index */
#define PALETTE_CODE_EXTENDED_LEN 24
void palette_code_extended(uint16_t count, char *buf, uint16_t fg, uint16_t bg);

/* Generic reset all attributes function */
#define PALETTE_RESET_LEN 5
void palette_reset(char *buf);

#endif
