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
	if (acs_chars == NULL && !(flags & CHARSET_FORCE)) {
		/* Terminal doesn't support ACS, bail out. */
		return NULL;
	}

	size_t count = 1; /* ASCII space is always available. */

	enum charset_flags enabled_flags = 0;

	if (acs_chars == NULL) {
		/* If terminfo doesn't support ACS but force is enabled,
		 * use the standard ANSI codes. */
		acs_ckboard_code[0] = 'a';
		if (flags & CHARSET_INVERSE) {
			acs_block_code[0] = '0';
		}
	} else {
		for (size_t i = 0; acs_chars[i]; i += 2) {
			switch (acs_chars[i]) {
			case 'a':
				acs_ckboard_code[0] = acs_chars[i + 1];
				count++;
				break;
			case '0':
				acs_block_code[0] = acs_chars[i + 1];
				count++;
				break;
			}
		}
	}

	/* We advertise inverse support only if the full block is available. */
	if ((flags & CHARSET_INVERSE) && acs_block_code[0] != '\0') {
		enabled_flags |= CHARSET_INVERSE;
	}

	struct charset *charset = malloc(sizeof(struct charset));

	charset->name = "acs";
	if (enabled_flags & CHARSET_INVERSE) {
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

	charset->glyph = glyph;

	if (acs_chars == NULL) {
		/* If terminfo doesn't support ACS but force is enabled,
		 * use the standard ANSI codes. */
		charset->enter = "\e[11m"; /* Select 1st alternate font */
		charset->exit = "\e[10m"; /* Select primary (default) font */
	} else {
		/* Use the terminfo definitions */
		charset->enter = enter_alt_charset_mode;
		if (!charset->enter)
			charset->enter = "";
		charset->exit = exit_alt_charset_mode;
		if (!charset->exit)
			charset->exit = "";
	}

	charset->num_glyphs = count;
	charset->flags = enabled_flags;

	return charset;
}
