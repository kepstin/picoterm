#include "charset.h"
#include "charset_internal.h"

#include <stdlib.h>

#include <glib/gi18n.h>

struct charset *charset_get_default(enum charset_flags flags)
{
	struct charset *charset = NULL;

	/* Mask out user-specified option flags.
	 * Only driver requests are allowed here. */
	flags &= CHARSET_INVERSE | CHARSET_RES_HALF | CHARSET_RES_QUARTER |
		CHARSET_SHADE;

	// TODO: Check if the user has specified a charset, and use it.
	// TODO: Set flags based on user-specified options
	
	/* If there was no user-configured charset, try loading something
	 * that is likely to work. In order of preferredness... */
	if (charset == NULL)
		charset = charset_get_utf8(flags);
	if (charset == NULL)
		charset = charset_get_acs(flags);
	if (charset == NULL)
		charset = charset_get_fallback(flags | CHARSET_FORCE);

	return charset;
}

void charset_free(struct charset *charset)
{
	free(charset->glyph);
	free(charset);
}

const char *charset_get_name(const struct charset *charset)
{
	return charset->name;
}

const char *charset_get_description(const struct charset *charset)
{
	return gettext(charset->description);
}

const struct glyph **charset_get_glyphs(
		const struct charset *charset,
		size_t *count)
{
	*count = charset->num_glyphs;
	return charset->glyph;
}

const char *charset_get_enter_string(const struct charset *charset)
{
	return charset->enter;
}

const char *charset_get_exit_string(const struct charset *charset)
{
	return charset->exit;
}

enum charset_flags charset_get_flags(const struct charset *charset)
{
	return charset->flags;
}
