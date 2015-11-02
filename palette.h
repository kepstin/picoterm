#ifndef PICOTERM_PALETTE_H
#define PICOTERM_PALETTE_H

#include <lcms2.h>
#include <stdint.h>

struct palette {
	const char *name;
	uint32_t fg_count;
	uint32_t bg_count;
	const cmsCIELab *(* fg_palette)(void);
	const cmsCIELab *(* bg_palette)(void);
	size_t escape_len;
	void (* code)(char *buf, uint32_t fg, uint32_t bg);
	void (* reset)(char *buf);
};

extern struct palette palette_256color;
extern struct palette palette_solarized;
extern struct palette palette_vga;

/* Shared helper functions used by multiple palettes */

/* For terminals that support 16 colors, but need 'intense' to select top
 * 8 foreground colors (and can't do 16 background colors) */
#define PALETTE_CODE_16COLOR_BOLD_LEN
void palette_code_16color_bold(char *buf, uint32_t fg, uint32_t bg);

/* For terminals that support addressing 16 colors independent of bold */
#define PALETTE_CODE_16COLOR_LEN 10
void palette_code_16color(char *buf, uint32_t fg, uint32_t bg);

/* For terminals that support addressing up to 256 colors via index */
#define PALETTE_CODE_EXTENDED_LEN 24
void palette_code_extended(uint32_t count, char *buf, uint32_t fg, uint32_t bg);

/* Generic reset all attributes function */
#define PALETTE_RESET_LEN 5
void palette_reset(char *buf);

#endif
