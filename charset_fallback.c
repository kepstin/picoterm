#include "charset.h"
#include "charset_internal.h"

#include <glib/gi18n.h>
#include <stdlib.h>

static const struct glyph fallback_space = {
	.code = " ",
	.weights = { { 0.00, 0.00 }, { 0.00, 0.00 } }
};

#define FALLBACK_GLYPH_LEN 1
static const struct glyph const *fallback_glyph[FALLBACK_GLYPH_LEN] = {
	&fallback_space
};

static const struct charset fallback_charset = {
	.name = "fallback",
	.description = N_("Fallback character set"),
	.glyph = NULL,
	.enter = "",
	.exit = "",
	.num_glyphs = FALLBACK_GLYPH_LEN,
	.flags = 0
};

struct charset *charset_get_fallback(enum charset_flags flags)
{
	struct charset *charset = malloc(sizeof(struct charset));
	memcpy(charset, &fallback_charset, sizeof(struct charset));

	const struct glyph **glyph = malloc(
			FALLBACK_GLYPH_LEN * sizeof(struct glyph *));
	memcpy(glyph, &fallback_glyph,
			FALLBACK_GLYPH_LEN * sizeof(struct glyph *));

	charset->glyph = glyph;

	return charset;
}
