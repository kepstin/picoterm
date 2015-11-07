#include "charset.h"
#include "charset_internal.h"

#include <stdlib.h>

const struct glyph **charset_get_glyphs(
		const struct charset *charset,
		size_t *count)
{
	*count = charset->num_glyphs;
	return charset->glyph;
}

const char *charset_get_enter_string(
		const struct charset *charset)
{
	return charset->enter;
}

const char *charset_get_exit_string(
		const struct charset *charset)
{
	return charset->exit;
}

void charset_free(
		struct charset *charset)
{
	free(charset->glyph);
	free(charset);
}
