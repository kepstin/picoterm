This is incomplete code. Please don't try to use it yet.

But, if you insist…

For the moment, the "picoterm" executable is the only one that builds and works.
Build it by running:

    make picoterm

In a fresh checkout it's set up to render a 160px wide raw RGB image. To make one,
open a picture in The Gimp, resize it to 160px wide, then Export As, pick
"Raw Image Data", and choose
- Standard (RGB RGB RGB …)
- R, G, B (normal)

Then run the picoterm tool with the filename of that image as the only command
line argument:

./picoterm some-image.data

Assuming you're running a truecolor-capable terminal (e.g. gnome-terminal), enjoy.
If you have a different terminal, well, you might have to edit the picoterm.c
code to swap out a different renderer and palette. YMMV. I'll probably add some
auto-detect or at least some command line parameters later.

Status:
- Truecolor mode uses ¼block characters in gnome-terminal and looks great.
- 256color palette uses blend characters to widen the pallete, and is ok in
  xterm and urxvt
- If needed, it can use half or even full block characters, and a 16 (or even
  8 + bold!) palette and render terrible images that are sort of recognizable.
