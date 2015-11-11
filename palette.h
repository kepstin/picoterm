#ifndef PICOTERM_PALETTE_H
#define PICOTERM_PALETTE_H

#include <stdint.h>
#include <glib.h>

struct palette {
	const char *name;
	guint16 fg_count;
	guint16 bg_count;
	const guint8 *(* fg_palette)(void);
	const guint8 *(* bg_palette)(void);
	gsize escape_len;
	void (* code)(char *buf, guint16 fg, guint16 bg);
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
