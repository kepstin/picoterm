.PHONY: default
default: all

PKG_CONFIG ?= pkg-config
CFLAGS ?= -O2 -ggdb

M_CFLAGS=-std=c11 -Wall $(shell $(PKG_CONFIG) --cflags glib-2.0)
M_LDFLAGS=-llcms2 -lcurses $(shell $(PKG_CONFIG) --libs glib-2.0)

.c.o:
	$(CC) $(CFLAGS) $(M_CFLAGS) -c $< -o $@

picoterm:
	$(CC) $(CFLAGS) $(M_CFLAGS) $(LDFLAGS) $(M_LDFLAGS) $^ -o $@

cursestest:
	$(CC) $(CFLAGS) $(M_CFLAGS) $(LDFLAGS) $(M_LDFLAGS) $^ -o $@

.PHONY: all
all: picoterm cursestest

charset.o: charset.c charset.h charset_internal.h
charset_acs.o: charset_acs.c charset.h charset_internal.h
charset_fallback.o: charset_fallback.c charset.h charset_internal.h
charset_utf8.o: charset_utf8.c charset.h charset_internal.h
driver.o: driver.c driver.h driver_internal.h
driver_rgb.o: driver_rgb.c driver.h driver_internal.h
driver_term_helper.o: driver_term_helper.c driver.h driver_internal.h
cursestest.o: cursestest.c charset.h driver.h
palette.o: palette.c palette.h
palette_256color.o: palette_256color.c palette.h
palette_rxvt.o: palette_rxvt.c palette.h
palette_solarized.o: palette_solarized.c palette.h
palette_tango.o: palette_tango.c palette.h
palette_vga.o: palette_vga.c palette.h
palette_xterm.o: palette_xterm.c palette.h
picoterm.o: picoterm.c palette.h
picoterm: palette.o palette_256color.o palette_rxvt.o palette_solarized.o \
		palette_tango.o palette_vga.o palette_xterm.o picoterm.o \
		charset.o charset_acs.o charset_utf8.o charset_fallback.o \
		driver_term_helper.o

cursestest: cursestest.o \
	charset.o charset_acs.o charset_fallback.o charset_utf8.o \
	driver.o driver_rgb.o driver_term_helper.o

