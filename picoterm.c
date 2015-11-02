#include "palette.h"

#include <lcms2.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>



int main(int argc, char *argv[]) {
	struct palette *palette = &palette_solarized;

	const cmsCIELab *Lab_palette;
	uint32_t count = palette->count;

	/* Load the xterm-256color palette */
	Lab_palette = palette->palette();

	char *code = malloc(palette->escape_len);
	char *reset = malloc(palette->escape_len);
	palette->reset(reset);

	for (unsigned i = 0; i < count; i++) {
		palette->code(code, i, i);
		printf("%u\t%s  %s\t%11.6f, %11.6f, %11.6f\n", i,
				code, reset,
				Lab_palette[i].L, Lab_palette[i].a, Lab_palette[i].b);
	}

	palette->code(code, 108, 138);
	printf("%s\342\226\200%s", code, reset);
	palette->code(code, 138, 108);
	printf("%s\342\226\200%s", code, reset);
	printf("\n");

	return 0;
}
