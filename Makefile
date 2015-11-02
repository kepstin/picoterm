.PHONY: default
default: all

CFLAGS=-O2

M_CFLAGS=-std=c11 -Wall
M_LDFLAGS=-llcms2

.c.o:
	$(CC) $(CFLAGS) $(M_CFLAGS) -c $< -o $@

picoterm: palette_256color.o palette_solarized.o picoterm.o
	$(CC) $(CFLAGS) $(M_CFLAGS) $(LDFLAGS) $(M_LDFLAGS) $^ -o $@

.PHONY: all
all: picoterm

palette_256color.o: palette_256color.c palette.h
palette_solarized.o: palette_solarized.c palette.h
picoterm.o: picoterm.c palette.h

