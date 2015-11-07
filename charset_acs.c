#include "charset.h"
#include "charset_internal.h"

#include <curses.h>
#include <term.h>

#include <stdlib.h>
#include <string.h>

#include <glib/gi18n.h>

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

struct charset *charset_get_acs(enum charset_flags flags) {
	size_t count = 1; /* We also use ASCII 'space' character */

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
	struct charset *charset = malloc(sizeof(struct charset));

	charset->name = "acs";
	if (flags & CHARSET_INVERSE) {
		charset->description =
			N_("ANSI Alternate Character Set with inverse");
	} else {
		charset->description =
			N_("ANSI Alternate Character Set");
	}
	

	const struct glyph **glyph = malloc(count * sizeof(struct glyph *));
	count = 0;

	glyph[count++] = &acs_space;
	if (acs_ckboard_code[0])
		glyph[count++] = &acs_ckboard;
	if (flags & CHARSET_INVERSE) {
		if (acs_block_code[0])
			glyph[count++] = &acs_block;
	}

	charset->num_glyphs = count;
	charset->glyph = glyph;
	charset->enter = enter_alt_charset_mode;
	charset->exit = exit_alt_charset_mode;

	return charset;
}
