#include "palette.h"

#include <lcms2.h>
#include <string.h>

static const cmsCIELab const palette[16] = {
	{ .L = 20, .a = -12, .b = -12 }, /* 0 - base03 */
	{ .L = 50, .a =  65, .b =  45 }, /* 1 - red */
	{ .L = 60, .a = -20, .b =  65 }, /* 2 - green */
	{ .L = 60, .a =  10, .b =  65 }, /* 3 - yellow */
	{ .L = 55, .a = -10, .b = -45 }, /* 4 - blue */
	{ .L = 50, .a =  65, .b = -5  }, /* 5 - magenta */
	{ .L = 60, .a = -35, .b = -5  }, /* 6 - cyan */
	{ .L = 92, .a =  0,  .b =  10 }, /* 7 - base2 */
	{ .L = 15, .a = -12, .b = -12 }, /* 8 - base03 */
	{ .L = 50, .a =  50, .b =  55 }, /* 9 - orange */
	{ .L = 45, .a = -7,  .b = -7  }, /* 10 - base01 */
	{ .L = 50, .a = -7,  .b = -7  }, /* 11 - base00 */
	{ .L = 60, .a = -6,  .b = -3  }, /* 12 - base0 */
	{ .L = 50, .a =  15, .b = -45 }, /* 13 - violet */
	{ .L = 65, .a = -5,  .b = -2  }, /* 14 - base1 */
	{ .L = 97, .a =  0,  .b =  10 }, /* 15 - base3 */
};

static const cmsCIELab *get_palette(void) {
	return palette;
}

static void code(char *buf, uint32_t fg, uint32_t bg) {
	if (fg >= 16 || bg >= 16) {
		buf[0] = '\0';
		return;
	}
	if (fg >= 8) fg += (60 - 8);
	fg += 30;
	if (bg >= 8) bg += (60 - 8);
	bg += 40;
	sprintf(buf, "\e[%u;%um", fg, bg);
}

static void reset(char *buf) {
	strcpy(buf, "\e[39;49m");
}

struct palette palette_solarized = {
	.name = "solarized",
	.palette = get_palette,
	.count = 16,
	.escape_len = 10,
	.code = code,
	.reset = reset
};

