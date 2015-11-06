#include "charset.h"

#include <curses.h>
#include <term.h>

#include <stdlib.h>

/* SPACE - full character box of background color */
static char acs_space_code[2] = { ' ', '\0' };
static const struct glyph acs_space = {
	.code = acs_space_code,
	.weights = { { 0.00, 0.00 }, { 0.00, 0.00 } }
};
/* CKBOARD - 50% blend of foreground/background */
static char acs_ckboard_code[2] = { '\0', '\0' };
static const struct glyph acs_ckboard = {
	.code = acs_ckboard_code,
	.weights = { { 0.50, 0.50 }, { 0.50, 0.50 } }
};
/* BLOCK - full character box of foreground color */
static char acs_block_code[2] = { '\0', '\0' };
static const struct glyph acs_block = {
	.code = acs_block_code,
	.weights = { { 1.00, 1.00 }, { 1.00, 1.00 } }
};

const struct glyph **charset_acs(size_t *countp, enum charset_flags flags) {
	size_t count = 1; /* Always have space */

	if (acs_chars != NULL && acs_chars != (char *) -1) {
		for (size_t i = 0; acs_chars[i]; i += 2) {
			if (acs_chars[i] == 'a') {
				acs_ckboard_code[0] = acs_chars[i + 1];
				count++;
			}
			if (flags & CHARSET_INVERSE) {
				if (acs_chars[i] == '0') {
					acs_block_code[0] = acs_chars[i + 1];
					count++;
				}
			}
		}
	}

	const struct glyph **charset = malloc(count * sizeof(struct glyph *));

	count = 0;

	charset[count++] = &acs_space;
	if (acs_ckboard_code[0])
		charset[count++] = &acs_ckboard;
	if (flags & CHARSET_INVERSE) {
		if (acs_block_code[0])
			charset[count++] = &acs_block;
	}

	*countp = count;
	return charset;
}
