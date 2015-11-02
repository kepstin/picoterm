#ifndef PICOTERM_PALETTE_H
#define PICOTERM_PALETTE_H

#include <lcms2.h>
#include <stdint.h>

struct palette {
	const char *name;
	const cmsCIELab *(* palette)(void);
	uint32_t count;
	size_t escape_len;
	void (* code)(char *buf, uint32_t fg, uint32_t bg);
	void (* reset)(char *buf);
};

extern struct palette palette_256color;
extern struct palette palette_solarized;

#endif
