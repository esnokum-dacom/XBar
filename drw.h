#ifndef DRW_H
#define DRW_H

#include <X11/X.h>
#include <X11/Xft/Xft.h>
#include <X11/Xlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  unsigned long colors[16];
  unsigned long background;
  unsigned long foreground;
} ColorScheme;

typedef struct {
    XftColor foreground;
    XftColor background;
    XftColor active;
    XftColor title;
} XftScheme;

unsigned long hex_to_xcolor(Display *dpy, const char *hex);

void load_colors(Display *dpy, ColorScheme *col);

void xcolor_to_xftcolor(Display *dpy, Visual *vis, Colormap cmap,
                        unsigned long pixel, XftColor *xft);

void load_xft_scheme(Display *dpy, Visual *vis, Colormap cmap, ColorScheme *col, XftScheme *scheme);

void free_xft_scheme(Display *dpy, Visual *vis, Colormap cmap, XftScheme *scheme);

void draw_wrapped_text(Display *dpy, XftDraw *xdraw, XftFont *font,
                       XftColor *color, int x, int y, int max_width,
                       const char *text);

int count_wrapped_lines(Display *dpy, XftFont *font, int max_width,
                        const char *text);

void draw_text(Display *dpy, XftDraw *xdraw, XftFont *font,
               XftColor *title_color, int x, int y, const char *title,
               int16_t type);
#endif
