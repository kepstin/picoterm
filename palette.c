#include "palette.h"

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void palette_code_16color_bold_blink(char *buf, uint16_t fg, uint16_t bg) {
	const char *bold = "22;";
	const char *blink = "25;";

	if (fg >= 16) {
		fg = 39;
	} else if (fg >= 8) {
		bold = "1;";
		fg = 30 + (fg - 8);
	} else {
		fg = 30 + fg;
	}

	if (bg >= 16) {
		bg = 49;
	} else if (bg >= 8) {
		blink = "5;";
		bg = 40 + (bg - 8);
	} else {
		bg = 40 + bg;
	}

	sprintf(buf, "\e[%s%s%u;%um", bold, blink, fg, bg);
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
